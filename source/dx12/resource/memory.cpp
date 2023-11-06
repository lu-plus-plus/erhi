#include "erhi/dx12/context/physical_device.hpp"
#include "erhi/dx12/context/device.hpp"

#include "erhi/dx12/resource/memory.hpp"

#include <bit>			// for memory type bits manipulation
#include <utility>		// for std::pair used in erhi/native mapping
#include <cassert>		// for assert()
#include <format>		// for std::format

#include "magic_enum.hpp"



namespace erhi::dx12 {

	D3D12_HEAP_TYPE MapMemoryHeapType(MemoryHeapType heapType) {
		switch (heapType) {
			case erhi::MemoryHeapType::Default:
				return D3D12_HEAP_TYPE_DEFAULT;
				break;
			case erhi::MemoryHeapType::Upload:
				return D3D12_HEAP_TYPE_UPLOAD;
				break;
			case erhi::MemoryHeapType::ReadBack:
			default:
				return D3D12_HEAP_TYPE_READBACK;
				break;
		}
	}



	/*
		In DX12, resources are divided into three categories: buffer, texture used as render target or depth stencil, and texture of other kinds.
		With D3D12_RESOURCE_HEAP_TIER_1, resources of different categories must be created upon different heaps.
		With D3D12_RESOURCE_HEAP_TIER_2, all kinds of resources may be mixed on a single heap.
	*/
	enum class ResourceCategory : uint32_t {
		Buffer = 0,
		RT_DS_Texture = 1,
		Non_RT_DS_Texture = 2,
		
		None = 0,					// For all kinds of placed resources on tier 2 device.
	};

	static D3D12_HEAP_FLAGS MapResourceCategoryToD3D12HeapFlags(Device * pDevice, ResourceCategory category) {
		if (pDevice->mPhysicalDeviceHandle->mFeatureD3D12Options.ResourceHeapTier == D3D12_RESOURCE_HEAP_TIER_1) {
			D3D12_HEAP_FLAGS heapFlags = D3D12_HEAP_FLAG_NONE;

			switch (category) {
				case ResourceCategory::Buffer:
					heapFlags |= D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;
					break;

				case ResourceCategory::RT_DS_Texture:
					heapFlags |= D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES;
					break;

				case ResourceCategory::Non_RT_DS_Texture:
					heapFlags |= D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
					break;

				default:
					break;
			}

			return heapFlags;
		}
		else {
			return D3D12_HEAP_FLAG_NONE;
		}		
	}

	static uint32_t EncodeMemoryTypeIndex(Device * pDevice, MemoryHeapType heapType, ResourceCategory category) {
		if (pDevice->mPhysicalDeviceHandle->mFeatureD3D12Options.ResourceHeapTier == D3D12_RESOURCE_HEAP_TIER_1) {
			return uint32_t(heapType) * uint32_t(magic_enum::enum_count<ResourceCategory>()) + uint32_t(category);
		}
		else {
			return uint32_t(heapType);
		}
	}

	static std::pair<MemoryHeapType, ResourceCategory> DecodeMemoryTypeIndex(Device * pDevice, uint32_t memoryTypeIndex) {
		if (pDevice->mPhysicalDeviceHandle->mFeatureD3D12Options.ResourceHeapTier == D3D12_RESOURCE_HEAP_TIER_1) {
			return {
				MemoryHeapType(memoryTypeIndex / magic_enum::enum_count<ResourceCategory>()),
				ResourceCategory(memoryTypeIndex % magic_enum::enum_count<ResourceCategory>())
			};
		}
		else {
			return {
				MemoryHeapType(memoryTypeIndex),
				ResourceCategory::None
			};
		}
	}



	static D3D12_RESOURCE_FLAGS MapBufferUsageFlags(BufferUsageFlags bufferUsage) {
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

		// In D3D12, resource flags are mostly for textures.
		if (bufferUsage & BufferUsageStorageBuffer) flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		return flags;
	}

	static D3D12_RESOURCE_DESC GetD3D12ResourceDesc(BufferDesc const & bufferDesc) {
		/*
			<todo>
			Per D3D12 document,
			"However, applications can use the most amount of capability support without concern about the efficiency impact on buffers."
			A test is needed to verify this statement.
			</todo>
		*/
		D3D12_RESOURCE_DESC const resourceDesc{
			.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
			.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
			.Width = bufferDesc.size,
			.Height = 1,
			.DepthOrArraySize = 1,
			.MipLevels = 1,
			.Format = DXGI_FORMAT_UNKNOWN,
			.SampleDesc = DXGI_SAMPLE_DESC{
				.Count = 1,
				.Quality = 0
			},
			.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
			.Flags = MapBufferUsageFlags(bufferDesc.usage)
		};
		return resourceDesc;
	}



	MemoryRequirements Device::GetBufferMemoryRequirements(MemoryHeapType heapType, BufferDesc const & bufferDesc) {
		uint32_t const memoryTypeIndex = EncodeMemoryTypeIndex(this, heapType, ResourceCategory::Buffer);
		
		D3D12_RESOURCE_DESC const resourceDesc = GetD3D12ResourceDesc(bufferDesc);

		D3D12_RESOURCE_ALLOCATION_INFO const allocationInfo = mpDevice->GetResourceAllocationInfo(0, 1, &resourceDesc);

		return MemoryRequirements{
			.memoryTypeBits = (1u << memoryTypeIndex),
			.prefersCommittedResource = false,
			.requiresCommittedResource = false,
			.pageTypeIndex = 0,
			.size = allocationInfo.SizeInBytes,
			.alignment = allocationInfo.Alignment
		};
	}



	Memory::Memory(Device * pDevice, MemoryRequirements const & requirements) : IMemory(requirements), mDeviceHandle(pDevice), mpAllocation(nullptr) {
		assert(std::has_single_bit(requirements.memoryTypeBits));
		
		// Map ERHI memory type index to ERHI heap type, and D3D12's "native memory categories".
		// Resources of different categories may NOT be allocated on the same heap.

		auto [heapType, memoryCategory] = DecodeMemoryTypeIndex(mDeviceHandle.get(), std::countr_zero(requirements.memoryTypeBits));

		D3D12MA::ALLOCATION_DESC allocDesc = {};
		allocDesc.HeapType = MapMemoryHeapType(heapType);
		allocDesc.ExtraHeapFlags = MapResourceCategoryToD3D12HeapFlags(mDeviceHandle.get(), memoryCategory);
		// D3D12MA does NOT clear new resources and new memorys by default. 
		// heapFlags |= D3D12_HEAP_FLAG_CREATE_NOT_ZEROED;
		// 
		// <todo>
		// Does atomic operation cost zero when it's not used at all?
		// 
		// heapFlags |= D3D12_HEAP_FLAG_ALLOW_SHADER_ATOMICS;
		// 
		// </todo>
		allocDesc.Flags = D3D12MA::ALLOCATION_FLAG_CAN_ALIAS;

		D3D12_RESOURCE_ALLOCATION_INFO allocInfo = {
			.SizeInBytes = requirements.size,
			.Alignment = requirements.alignment
		};

		D3D12CheckResult(pDevice->mpMemoryAllocator->AllocateMemory(&allocDesc, &allocInfo, &mpAllocation));
	}

	Memory::~Memory() {
		mpAllocation->Release();
	}

	IDeviceHandle Memory::GetDevice() const {
		return mDeviceHandle;
	}

	IMemoryHandle Device::AllocateMemory(MemoryRequirements const & requirements) {
		return MakeHandle<Memory>(this, requirements);
	}



	D3D12_RESOURCE_STATES GetInitialState(D3D12_HEAP_TYPE heapType) {
		D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

		switch (heapType) {
			case D3D12_HEAP_TYPE_DEFAULT: {
				state = D3D12_RESOURCE_STATE_COMMON;
			} break;

			case D3D12_HEAP_TYPE_UPLOAD: {
				state = D3D12_RESOURCE_STATE_GENERIC_READ;
			} break;

			case D3D12_HEAP_TYPE_READBACK: {
				state = D3D12_RESOURCE_STATE_COPY_DEST;
			} break;

			default: break;
		}

		return state;
	}



	Buffer::Buffer(Device * pDevice, MemoryHeapType heapType, BufferDesc const & desc) : IBuffer(desc), mDeviceHandle(pDevice), mpAllocation(nullptr), mpResource(nullptr) {
		D3D12MA::ALLOCATION_DESC allocDesc = {};
		allocDesc.HeapType = MapMemoryHeapType(heapType);
		
		auto const resourceDesc = GetD3D12ResourceDesc(desc);
		auto const initialState = GetInitialState(allocDesc.HeapType);

		D3D12CheckResult(pDevice->mpMemoryAllocator->CreateResource(&allocDesc, &resourceDesc, initialState, nullptr, &mpAllocation, IID_PPV_ARGS(&mpResource)));
	}

	Buffer::~Buffer() {
		mpResource->Release();
		mpAllocation->Release();
	}

	IBufferHandle Device::CreateBuffer(MemoryHeapType heapType, BufferDesc const & bufferDesc) {
		return MakeHandle<Buffer>(this, heapType, bufferDesc);
	}



	PlacedBuffer::PlacedBuffer(Memory * pMemory, uint64_t offset, BufferDesc const & desc) : IBuffer(desc), mMemoryHandle(pMemory), mpBuffer(nullptr) {
		assert(pMemory != nullptr);
		assert(pMemory->mRequirements.size >= offset + desc.size);

		auto [heapType, memoryCategory] = DecodeMemoryTypeIndex(pMemory->mDeviceHandle.get(), std::countr_zero(pMemory->mRequirements.memoryTypeBits));

		auto const resourceDesc = GetD3D12ResourceDesc(desc);
		auto const initialState = GetInitialState(MapMemoryHeapType(heapType));

		D3D12CheckResult(pMemory->mDeviceHandle->mpMemoryAllocator->CreateAliasingResource(pMemory->mpAllocation, offset, &resourceDesc, initialState, nullptr, IID_PPV_ARGS(&mpBuffer)));
	}

	PlacedBuffer::~PlacedBuffer() {
		mpBuffer->Release();
	}

	IBufferHandle Device::CreatePlacedBuffer(IMemoryHandle memoryHandle, uint64_t offset, BufferDesc const & bufferDesc) {
		return MakeHandle<PlacedBuffer>(dynamic_cast<Memory *>(memoryHandle.get()), offset, bufferDesc);
	}



	DXGI_FORMAT MapFormat(Format format) {
		switch (format) {
			case erhi::Format::Unknown:
				return DXGI_FORMAT_UNKNOWN;
				break;

			case erhi::Format::R32G32B32A32_Typeless:
				return DXGI_FORMAT_R32G32B32A32_TYPELESS;
				break;
			case erhi::Format::R32G32B32A32_Float:
				return DXGI_FORMAT_R32G32B32A32_FLOAT;
				break;
			case erhi::Format::R32G32B32A32_UInt:
				return DXGI_FORMAT_R32G32B32A32_UINT;
				break;
			case erhi::Format::R32G32B32A32_SInt:
				return DXGI_FORMAT_R32G32B32A32_SINT;
				break;

			case erhi::Format::R32G32B32_Typeless:
				return DXGI_FORMAT_R32G32B32_TYPELESS;
				break;
			case erhi::Format::R32G32B32_Float:
				return DXGI_FORMAT_R32G32B32_FLOAT;
				break;
			case erhi::Format::R32G32B32_UInt:
				return DXGI_FORMAT_R32G32B32_UINT;
				break;
			case erhi::Format::R32G32B32_SInt:
				return DXGI_FORMAT_R32G32B32_SINT;
				break;

			case erhi::Format::R16G16B16A16_Typeless:
				return DXGI_FORMAT_R16G16B16A16_TYPELESS;
				break;
			case erhi::Format::R16G16B16A16_Float:
				return DXGI_FORMAT_R16G16B16A16_FLOAT;
				break;
			case erhi::Format::R16G16B16A16_UNorm:
				return DXGI_FORMAT_R16G16B16A16_UNORM;
				break;
			case erhi::Format::R16G16B16A16_UInt:
				return DXGI_FORMAT_R16G16B16A16_UINT;
				break;
			case erhi::Format::R16G16B16A16_SNorm:
				return DXGI_FORMAT_R16G16B16A16_SNORM;
				break;
			case erhi::Format::R16G16B16A16_SInt:
				return DXGI_FORMAT_R16G16B16A16_SINT;
				break;

			case erhi::Format::R32G32_Typeless:
				return DXGI_FORMAT_R32G32_TYPELESS;
				break;
			case erhi::Format::R32G32_Float:
				return DXGI_FORMAT_R32G32_FLOAT;
				break;
			case erhi::Format::R32G32_UInt:
				return DXGI_FORMAT_R32G32_UINT;
				break;
			case erhi::Format::R32G32_SInt:
				return DXGI_FORMAT_R32G32_SINT;
				break;

			case erhi::Format::R8G8B8A8_Typeless:
				return DXGI_FORMAT_R8G8B8A8_TYPELESS;
				break;
			case erhi::Format::R8G8B8A8_UNorm:
				return DXGI_FORMAT_R8G8B8A8_UNORM;
				break;
			case erhi::Format::R8G8B8A8_UInt:
				return DXGI_FORMAT_R8G8B8A8_UINT;
				break;
			case erhi::Format::R8G8B8A8_SNorm:
				return DXGI_FORMAT_R8G8B8A8_SNORM;
				break;
			case erhi::Format::R8G8B8A8_SInt:
				return DXGI_FORMAT_R8G8B8A8_SINT;
				break;

			case erhi::Format::R16G16_Typeless:
				return DXGI_FORMAT_R16G16_TYPELESS;
				break;
			case erhi::Format::R16G16_Float:
				return DXGI_FORMAT_R16G16_FLOAT;
				break;
			case erhi::Format::R16G16_UNorm:
				return DXGI_FORMAT_R16G16_UNORM;
				break;
			case erhi::Format::R16G16_UInt:
				return DXGI_FORMAT_R16G16_UINT;
				break;
			case erhi::Format::R16G16_SNorm:
				return DXGI_FORMAT_R16G16_SNORM;
				break;
			case erhi::Format::R16G16_SInt:
				return DXGI_FORMAT_R16G16_SINT;
				break;

			case erhi::Format::R32_Float:
				return DXGI_FORMAT_R32_FLOAT;
				break;
			case erhi::Format::R32_UInt:
				return DXGI_FORMAT_R32_UINT;
				break;
			case erhi::Format::R32_SInt:
				return DXGI_FORMAT_R32_SINT;
				break;
			
			case erhi::Format::D32_Float:
				return DXGI_FORMAT_D32_FLOAT;
				break;
			case erhi::Format::D16_UNorm:
				return DXGI_FORMAT_D16_UNORM;
				break;
			case erhi::Format::D24_UNorm_S8_UInt:
				return DXGI_FORMAT_D24_UNORM_S8_UINT;
				break;

			default:
				return DXGI_FORMAT_UNKNOWN;
				break;
		}
	}



	static uint32_t MapSampleCount(TextureSampleCount sampleCount) {
		return 1u << static_cast<uint32_t>(sampleCount);
	}

	[[maybe_unused]]
	static uint64_t GetDefaultTextureAlignment(TextureSampleCount sampleCount) {
		return sampleCount == TextureSampleCount::Count_1
			? D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT
			: D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT;
	}

	static uint64_t GetSmallTextureAlignment(TextureSampleCount sampleCount) {
		return sampleCount == TextureSampleCount::Count_1
			? D3D12_SMALL_RESOURCE_PLACEMENT_ALIGNMENT
			: D3D12_SMALL_MSAA_RESOURCE_PLACEMENT_ALIGNMENT;
	}

	static D3D12_RESOURCE_FLAGS MapTextureUsageFlags(TextureUsageFlags flags) {
		D3D12_RESOURCE_FLAGS result = D3D12_RESOURCE_FLAG_NONE;

		if (flags & TextureUsageStorage) {
			result |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}
		if (flags & TextureUsageRenderTargetAttachment) {
			result |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		}
		if (flags & TextureUsageDepthStencilAttachment) {
			result |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		}

		return result;
	}



	static D3D12_RESOURCE_DESC GetD3D12ResourceDesc(Device * pDevice, TextureDesc const & desc) {
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

			default:
				break;
		}

		// In allocating placed resources with D3D12MA, the alignments and offsets
		// are still required to be specified explicitly by user.
		uint64_t const smallTextureAlignment = GetSmallTextureAlignment(desc.sampleCount);

		DXGI_FORMAT const format = MapFormat(desc.format);
		uint32_t const sampleCount = MapSampleCount(desc.sampleCount);
		{
			D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msaaQualityLevels{
				.Format = format,
				.SampleCount = sampleCount
			};
			D3D12CheckResult(pDevice->mpDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msaaQualityLevels, sizeof(msaaQualityLevels)));
			if (msaaQualityLevels.NumQualityLevels == 0) {
				throw invalid_arguments(std::format(
					"Texture with DXGI_FORMAT = {} and sample count = {} is not supported.",
					magic_enum::enum_name(format), sampleCount
				));
			}
		}

		D3D12_RESOURCE_DESC resourceDesc = {
			.Dimension = dimension,
			.Alignment = smallTextureAlignment,
			.Width = desc.extent[0],
			.Height = desc.extent[1],
			.DepthOrArraySize = uint16_t(desc.extent[2]),
			.MipLevels = uint16_t(desc.mipLevels),
			.Format = format,
			.SampleDesc = DXGI_SAMPLE_DESC{
				.Count = sampleCount,
				.Quality = 0
			},
			.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
			.Flags = MapTextureUsageFlags(desc.usage)
		};

		D3D12_RESOURCE_ALLOCATION_INFO const smallTextureAllocationInfo = pDevice->mpDevice->GetResourceAllocationInfo(0, 1, &resourceDesc);

		// If the alignment requested is not granted, then let D3D tell us the alignment that needs to be used for these resources.
		// https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/Samples/Desktop/D3D12SmallResources/src/D3D12SmallResources.cpp
		if (smallTextureAllocationInfo.Alignment != smallTextureAlignment) {
			resourceDesc.Alignment = 0;
		}

		return resourceDesc;
	}



	MemoryRequirements Device::GetTextureMemoryRequirements(MemoryHeapType heapType, TextureDesc const & textureDesc) {
		uint32_t memoryTypeIndex = 0xFFFF'FFFFu;
		if (textureDesc.usage & (TextureUsageRenderTargetAttachment | TextureUsageDepthStencilAttachment)) {
			memoryTypeIndex = EncodeMemoryTypeIndex(this, heapType, ResourceCategory::RT_DS_Texture);
		}
		else {
			memoryTypeIndex = EncodeMemoryTypeIndex(this, heapType, ResourceCategory::Non_RT_DS_Texture);
		}

		D3D12_RESOURCE_DESC resourceDesc = GetD3D12ResourceDesc(this, textureDesc);
		D3D12_RESOURCE_ALLOCATION_INFO allocationInfo = mpDevice->GetResourceAllocationInfo(0, 1, &resourceDesc);
		
		return MemoryRequirements{
			.memoryTypeBits = (1u << memoryTypeIndex),
			.prefersCommittedResource = false,
			.requiresCommittedResource = false,
			.size = allocationInfo.SizeInBytes,
			.alignment = allocationInfo.Alignment,
		};
	}



	Texture::Texture(Device * pDevice, MemoryHeapType heapType, TextureDesc const & desc) : ITexture(desc), mDeviceHandle(pDevice), mpAllocation(nullptr), mpResource(nullptr) {
		D3D12MA::ALLOCATION_DESC allocDesc = {};
		allocDesc.HeapType = MapMemoryHeapType(heapType);

		auto const resourceDesc = GetD3D12ResourceDesc(pDevice, desc);
		auto const initialState = GetInitialState(allocDesc.HeapType);

		D3D12CheckResult(pDevice->mpMemoryAllocator->CreateResource(&allocDesc, &resourceDesc, initialState, nullptr, &mpAllocation, IID_PPV_ARGS(&mpResource)));
	}

	Texture::~Texture() {
		mpResource->Release();
		mpAllocation->Release();
	}

	ITextureHandle Device::CreateTexture(MemoryHeapType heapType, TextureDesc const & textureDesc) {
		return MakeHandle<Texture>(this, heapType, textureDesc);
	}



	PlacedTexture::PlacedTexture(Memory * pMemory, uint64_t offset, TextureDesc const & desc) : ITexture(desc), mMemoryHandle(pMemory), mpTexture(nullptr) {
		assert(pMemory != nullptr);
		
		auto [heapType, memoryCategory] = DecodeMemoryTypeIndex(pMemory->mDeviceHandle.get(), std::countr_zero(pMemory->mRequirements.memoryTypeBits));

		auto const resourceDesc = GetD3D12ResourceDesc(pMemory->mDeviceHandle.get(), desc);
		auto const initialState = GetInitialState(MapMemoryHeapType(heapType));

		D3D12CheckResult(pMemory->mDeviceHandle->mpMemoryAllocator->CreateAliasingResource(pMemory->mpAllocation, offset, &resourceDesc, initialState, nullptr, IID_PPV_ARGS(&mpTexture)));
	}

	PlacedTexture::~PlacedTexture() {
		mpTexture->Release();
	}

	ITextureHandle Device::CreatePlacedTexture(IMemoryHandle memoryHandle, uint64_t offset, TextureDesc const & desc) {
		return MakeHandle<PlacedTexture>(dynamic_cast<Memory *>(memoryHandle.get()), offset, desc);
	}

}