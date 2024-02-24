#include "erhi/dx12/context/context.hpp"
#include "erhi/dx12/resource/resource.hpp"

#include <cassert>



namespace erhi::dx12 {

	TextureView::TextureView(ITextureHandle pTexture, TextureViewDesc const & desc) : ITextureView(desc), mpTexture(dynamic_cast<TextureHandle>(pTexture)) {}

	ITextureViewHandle Device::CreateTextureView(ITextureHandle pTexture, TextureViewDesc const & desc) {
		return new TextureView(pTexture, desc);
	}



	D3D12_DESCRIPTOR_HEAP_TYPE MapDescriptorHeapType(DescriptorHeapType descriptorHeapType) {
		return descriptorHeapType == DescriptorHeapType::CBV_SRV_UAV
			? D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
			: D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	}

	DescriptorHeapBase::DescriptorHeapBase(Device * pDevice, DescriptorHeapDesc const & desc, D3D12_DESCRIPTOR_HEAP_FLAGS flags) :
		mpDevice(pDevice), mDescriptorSizeInBytes(0), mHeapStartCPU{}, mHeapStartGPU{} {
		
		D3D12_DESCRIPTOR_HEAP_TYPE const heapType = MapDescriptorHeapType(desc.type);

		mDescriptorSizeInBytes = pDevice->GetDescriptorHandleIncrementSize(heapType);

		D3D12_DESCRIPTOR_HEAP_DESC d3d12Desc = {
			.Type = heapType,
			.NumDescriptors = UINT(desc.sizeInBytes / mDescriptorSizeInBytes),
			.Flags = flags,
			.NodeMask = 0
		};

		D3D12CheckResult(pDevice->mpDevice->CreateDescriptorHeap(&d3d12Desc, IID_PPV_ARGS(mpHeap.GetAddressOf())));

		mHeapStartCPU = mpHeap->GetCPUDescriptorHandleForHeapStart();
		
		if (flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
			mHeapStartGPU = mpHeap->GetGPUDescriptorHandleForHeapStart();
	}

	DescriptorHeapBase::~DescriptorHeapBase() {
		mpHeap.Reset();
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHeapBase::CPUDescriptorHandleAt(uint64_t offsetInBytes) const & {
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(mHeapStartCPU, offsetInBytes / mDescriptorSizeInBytes, mDescriptorSizeInBytes);
	}

	CD3DX12_GPU_DESCRIPTOR_HANDLE DescriptorHeapBase::GPUDescriptorHandleAt(uint64_t offsetInBytes) const & {
		return CD3DX12_GPU_DESCRIPTOR_HANDLE(mHeapStartGPU, offsetInBytes / mDescriptorSizeInBytes, mDescriptorSizeInBytes);
	}



	ICPUDescriptorHeap * Device::CreateCPUDescriptorHeap(DescriptorHeapDesc const & desc) {
		return new CPUDescriptorHeap(this, desc);
	}

	IGPUDescriptorHeap * Device::CreateGPUDescriptorHeap(DescriptorHeapDesc const & desc) {
		return new GPUDescriptorHeap(this, desc);
	}



	void CPUDescriptorHeap::CreateBufferShaderResourceView(uint64_t offsetInBytes, IBufferHandle pBuffer, BufferDescriptorDesc const & desc) {
		assert(not(desc.format != Format::Unknown and desc.structureSizeInBytes != 0));

		D3D12_SHADER_RESOURCE_VIEW_DESC const srvDesc{
			.Format = mapping::MapFormat(desc.format),
			.ViewDimension = D3D12_SRV_DIMENSION_BUFFER,
			.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
			.Buffer = D3D12_BUFFER_SRV{
				.FirstElement = desc.offsetInElements,
				.NumElements = UINT(desc.countInElements),
				.StructureByteStride = desc.structureSizeInBytes,
				.Flags = desc.flagBits == BufferDescriptorAllowByteAddressBuffer ? D3D12_BUFFER_SRV_FLAG_RAW : D3D12_BUFFER_SRV_FLAG_NONE,
			}
		};

		mpDevice->mpDevice->CreateShaderResourceView(
			dynamic_cast<BufferHandle>(pBuffer)->mpResource.Get(),
			&srvDesc,
			CPUDescriptorHandleAt(offsetInBytes)
		);
	}

	void CPUDescriptorHeap::CreateBufferUnorderedAccessView(uint64_t offsetInBytes, IBufferHandle pBuffer, BufferDescriptorDesc const & desc) {
		assert(not(desc.format != Format::Unknown and desc.structureSizeInBytes != 0));
		
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {
			.Format = mapping::MapFormat(desc.format),
			.ViewDimension = D3D12_UAV_DIMENSION_BUFFER,
			.Buffer = D3D12_BUFFER_UAV{
				.FirstElement = desc.offsetInElements,
				.NumElements = UINT(desc.countInElements),
				.StructureByteStride = desc.structureSizeInBytes,
				.CounterOffsetInBytes = 0,
				.Flags = desc.flagBits == BufferDescriptorAllowByteAddressBuffer ? D3D12_BUFFER_UAV_FLAG_RAW : D3D12_BUFFER_UAV_FLAG_NONE
			}
		};

		mpDevice->mpDevice->CreateUnorderedAccessView(
			dynamic_cast<BufferHandle>(pBuffer)->mpResource.Get(),
			nullptr /* the Counter Resource is not used */,
			&uavDesc,
			CPUDescriptorHandleAt(offsetInBytes)
		);
	}



	D3D12_SRV_DIMENSION MapSRVDimension(TextureViewDimension dimension) {
		switch (dimension) {
			case TextureViewDimension::Texture1D: return D3D12_SRV_DIMENSION_TEXTURE1D;
			case TextureViewDimension::Texture2D: return D3D12_SRV_DIMENSION_TEXTURE2D;
			case TextureViewDimension::Texture3D: return D3D12_SRV_DIMENSION_TEXTURE3D;
		}
		return D3D12_SRV_DIMENSION_UNKNOWN;
	}

	void CPUDescriptorHeap::CreateTextureShaderResourceView(uint64_t offsetInBytes, ITextureViewHandle pTextureView) {
		TextureView & textureView = *dynamic_cast<TextureViewHandle>(pTextureView);
		
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{
			.Format = mapping::MapFormat(textureView.mDesc.format),
			.ViewDimension = MapSRVDimension(textureView.mDesc.dimension),
			.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING
		};
		
		switch (textureView.mDesc.dimension) {
			case TextureViewDimension::Texture1D:
				srvDesc.Texture1D.MipLevels = textureView.mDesc.mipLevelCount;
				srvDesc.Texture1D.MostDetailedMip = textureView.mDesc.mostDetailedMipLevel;
				break;
			case TextureViewDimension::Texture2D:
				srvDesc.Texture2D.MipLevels = textureView.mDesc.mipLevelCount;
				srvDesc.Texture2D.MostDetailedMip = textureView.mDesc.mostDetailedMipLevel;
				break;
			case TextureViewDimension::Texture3D:
				srvDesc.Texture3D.MipLevels = textureView.mDesc.mipLevelCount;
				srvDesc.Texture3D.MostDetailedMip = textureView.mDesc.mostDetailedMipLevel;
				break;
		}

		mpDevice->mpDevice->CreateShaderResourceView(
			textureView.mpTexture->mpResource.Get(),
			&srvDesc,
			CPUDescriptorHandleAt(offsetInBytes)
		);
	}



	D3D12_UAV_DIMENSION MapUAVDimension(TextureViewDimension dimension) {
		switch (dimension) {
			case TextureViewDimension::Texture1D: return D3D12_UAV_DIMENSION_TEXTURE1D;
			case TextureViewDimension::Texture2D: return D3D12_UAV_DIMENSION_TEXTURE2D;
			case TextureViewDimension::Texture3D: return D3D12_UAV_DIMENSION_TEXTURE3D;
		}
		return D3D12_UAV_DIMENSION_UNKNOWN;
	}

	void CPUDescriptorHeap::CreateTextureUnorderedAccessView(uint64_t offsetInBytes, ITextureViewHandle pTextureView) {
		TextureView & textureView = *dynamic_cast<TextureViewHandle>(pTextureView);

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{
			.Format = mapping::MapFormat(textureView.mDesc.format),
			.ViewDimension = MapUAVDimension(textureView.mDesc.dimension)
		};

		switch (textureView.mDesc.dimension) {
			case TextureViewDimension::Texture1D:
				uavDesc.Texture1D.MipSlice = textureView.mDesc.mostDetailedMipLevel;
				break;
			case TextureViewDimension::Texture2D:
				uavDesc.Texture2D.MipSlice = textureView.mDesc.mostDetailedMipLevel;
				break;
			case TextureViewDimension::Texture3D:
				uavDesc.Texture3D.MipSlice = textureView.mDesc.mostDetailedMipLevel;
				uavDesc.Texture3D.FirstWSlice = 0;
				uavDesc.Texture3D.WSize = -1;
				break;
		}

		mpDevice->mpDevice->CreateUnorderedAccessView(
			dynamic_cast<TextureHandle>(pTextureView)->mpResource.Get(),
			nullptr /* the Counter Resource is not used */,
			&uavDesc,
			CPUDescriptorHandleAt(offsetInBytes)
		);
	}



	IDescriptorSetLayoutHandle Device::CreateDescriptorSetLayout(DescriptorSetLayoutDesc const & desc) {
		return new DescriptorSetLayout(desc);
	}

	uint64_t Device::GetDescriptorSetLayoutSize(IDescriptorSetLayoutHandle iLayout) {
		auto layout = dynamic_cast<DescriptorSetLayoutHandle>(iLayout);
		
		uint64_t totalCount = 0;
		for (auto const & binding : layout->mBindings) {
			totalCount += binding.descriptorCount;
		}

		UINT const descriptorSize = GetDescriptorHandleIncrementSize(MapDescriptorHeapType(layout->mDescriptorHeapType));

		return totalCount * descriptorSize;
	}

	uint64_t Device::GetDescriptorSetLayoutBindingOffset(IDescriptorSetLayoutHandle iLayout, uint64_t binding) {
		auto layout = dynamic_cast<DescriptorSetLayoutHandle>(iLayout);

		assert(binding < layout->mBindings.size());

		uint64_t totalCount = 0;
		for (uint32_t i = 0; i < binding; ++i) {
			totalCount += layout->mBindings[i].descriptorCount;
		}

		UINT const descriptorSize = GetDescriptorHandleIncrementSize(MapDescriptorHeapType(layout->mDescriptorHeapType));

		return totalCount * descriptorSize;
	}

}