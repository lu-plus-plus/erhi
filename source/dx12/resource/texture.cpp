#include "erhi/dx12/context/context.hpp"
#include "erhi/dx12/resource/resource.hpp"

#include <format>			// for format

#include "magic_enum.hpp"	// for enum_name



namespace erhi::dx12 {

	uint32_t MapSampleCount(TextureSampleCount sampleCount) {
		return 1u << static_cast<uint32_t>(sampleCount);
	}

	[[maybe_unused]]
	uint64_t GetDefaultTextureAlignment(TextureSampleCount sampleCount) {
		return sampleCount == TextureSampleCount::Count_1
			? D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT
			: D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT;
	}

	uint64_t GetSmallTextureAlignment(TextureSampleCount sampleCount) {
		return sampleCount == TextureSampleCount::Count_1
			? D3D12_SMALL_RESOURCE_PLACEMENT_ALIGNMENT
			: D3D12_SMALL_MSAA_RESOURCE_PLACEMENT_ALIGNMENT;
	}

	D3D12_RESOURCE_FLAGS MapTextureUsageFlags(TextureUsageFlags flags) {
		D3D12_RESOURCE_FLAGS result = D3D12_RESOURCE_FLAG_NONE;

		if (flags & TextureUsageUnorderedAccess) {
			result |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}
		if (flags & TextureUsageRenderTarget) {
			result |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		}
		if (flags & TextureUsageDepthStencil) {
			result |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
			if (not (flags & TextureUsageShaderResource)) {
				result |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
			}
		}

		return result;
	}



	D3D12_RESOURCE_DESC1 GetD3D12ResourceDesc(ID3D12DeviceLatest * pDevice, TextureDesc const & desc) {
		D3D12_RESOURCE_DIMENSION dimension = D3D12_RESOURCE_DIMENSION_UNKNOWN;
		switch (desc.dimension) {
			case TextureDimension::Texture1D:
				dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
				break;

			case TextureDimension::Texture2D:
				dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				break;

			case TextureDimension::Texture3D:
				dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
				break;
		}

		// In allocating placed resources with D3D12MA, the alignments and offsets
		// are still required to be specified explicitly by user.
		uint64_t const smallTextureAlignment = GetSmallTextureAlignment(desc.sampleCount);

		DXGI_FORMAT const format = mapping::MapFormat(desc.format);
		uint32_t const sampleCount = MapSampleCount(desc.sampleCount);
		{
			D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msaaQualityLevels{
				.Format = format,
				.SampleCount = sampleCount
			};
			D3D12CheckResult(pDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msaaQualityLevels, sizeof(msaaQualityLevels)));
			if (msaaQualityLevels.NumQualityLevels == 0) {
				throw std::invalid_argument(std::format(
					"Texture with DXGI_FORMAT = {} and sample count = {} is not supported.",
					magic_enum::enum_name(format), sampleCount
				));
			}
		}

		D3D12_RESOURCE_DESC1 resourceDesc = {
			.Dimension = dimension,
			.Alignment = smallTextureAlignment,
			.Width = desc.extent[0],
			.Height = desc.extent[1],
			.DepthOrArraySize = UINT16(desc.extent[2]),
			.MipLevels = UINT16(desc.mipLevels),
			.Format = format,
			.SampleDesc = DXGI_SAMPLE_DESC{
				.Count = sampleCount,
				.Quality = 0
			},
			.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
			.Flags = MapTextureUsageFlags(desc.usage),
			.SamplerFeedbackMipRegion = D3D12_MIP_REGION{ 0, 0, 0, }
		};

		D3D12_RESOURCE_ALLOCATION_INFO1 placedResourceAllocationInfo = {};
		D3D12_RESOURCE_ALLOCATION_INFO const smallTextureAllocationInfo = pDevice->GetResourceAllocationInfo2(0, 1, &resourceDesc, &placedResourceAllocationInfo);

		// If the alignment requested is not granted, then let D3D tell us the alignment that needs to be used for these resources.
		// https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/Samples/Desktop/D3D12SmallResources/src/D3D12SmallResources.cpp
		if (resourceDesc.Alignment != smallTextureAllocationInfo.Alignment) {
			resourceDesc.Alignment = 0;
		}

		return resourceDesc;
	}



	D3D12_BARRIER_LAYOUT MapTextureLayout(TextureLayout layout) {
		switch (layout) {
			case TextureLayout::Undefined: return D3D12_BARRIER_LAYOUT_UNDEFINED;
			case TextureLayout::Common: return D3D12_BARRIER_LAYOUT_COMMON;
			case TextureLayout::Present: return D3D12_BARRIER_LAYOUT_PRESENT;
			case TextureLayout::ShaderResource: return D3D12_BARRIER_LAYOUT_SHADER_RESOURCE;
			case TextureLayout::UnorderedAccess: return D3D12_BARRIER_LAYOUT_UNORDERED_ACCESS;
			case TextureLayout::RenderTarget: return D3D12_BARRIER_LAYOUT_RENDER_TARGET;
			case TextureLayout::DepthStencilWrite: return D3D12_BARRIER_LAYOUT_DEPTH_STENCIL_READ;
			case TextureLayout::DepthStencilRead: return D3D12_BARRIER_LAYOUT_DEPTH_STENCIL_WRITE;
			case TextureLayout::CopySource: return D3D12_BARRIER_LAYOUT_COPY_SOURCE;
			case TextureLayout::CopyTarget: return D3D12_BARRIER_LAYOUT_COPY_DEST;
		}
		return D3D12_BARRIER_LAYOUT_UNDEFINED;
	}



	Texture::Texture(Device * pDevice, MemoryHeapType heapType, TextureDesc const & desc) : ITexture(desc), mpAllocation(nullptr), mpResource(nullptr) {
		D3D12MA::ALLOCATION_DESC allocationDesc = {};
		allocationDesc.HeapType = mapping::MapMemoryHeapType(heapType);

		auto const resourceDesc = GetD3D12ResourceDesc(pDevice->mpDevice.Get(), desc);
		D3D12_BARRIER_LAYOUT const initialLayout = MapTextureLayout(desc.initialLayout);
		
		D3D12CheckResult(pDevice->mpMemoryAllocator->CreateResource3(&allocationDesc, &resourceDesc, initialLayout, nullptr, 0, nullptr, mpAllocation.GetAddressOf(), IID_PPV_ARGS(mpResource.GetAddressOf())));
	}

	Texture::Texture(ComPtr<ID3D12Resource> pResource, TextureDesc const & desc) : ITexture(desc), mpAllocation(nullptr), mpResource(pResource) {}

	Texture::~Texture() {
		mpResource.Reset();
		mpAllocation.Reset();
	}



	ITextureHandle Device::CreateTexture(MemoryHeapType heapType, TextureDesc const & textureDesc) {
		return new Texture(this, heapType, textureDesc);
	}

}