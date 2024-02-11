#pragma	once

#include "../../common/resource/descriptor.hpp"
#include "../native.hpp"



namespace erhi::dx12 {

	struct TextureView : ITextureView {
		TextureHandle mpTexture;

		TextureView(ITextureHandle pTexture, TextureViewDesc const & desc);
		virtual ~TextureView() = default;
	};

	D3D12_DESCRIPTOR_HEAP_TYPE MapDescriptorHeapType(DescriptorHeapType descriptorHeapType);

	struct DescriptorHeapBase {
		Device * mpDevice;
		ComPtr<ID3D12DescriptorHeap> mpHeap;
		UINT mDescriptorSizeInBytes;
		D3D12_CPU_DESCRIPTOR_HANDLE mHeapStartCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE mHeapStartGPU;

		DescriptorHeapBase(Device * pDevice, DescriptorHeapDesc const & desc, D3D12_DESCRIPTOR_HEAP_FLAGS flags);
		~DescriptorHeapBase();

		CD3DX12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandleAt(uint64_t offsetInBytes) const &;
		CD3DX12_GPU_DESCRIPTOR_HANDLE GPUDescriptorHandleAt(uint64_t offsetInBytes) const &;
	};

	struct CPUDescriptorHeap : DescriptorHeapBase, ICPUDescriptorHeap {
		CPUDescriptorHeap(Device * pDevice, DescriptorHeapDesc const & desc) : DescriptorHeapBase(pDevice, desc, D3D12_DESCRIPTOR_HEAP_FLAG_NONE), ICPUDescriptorHeap(desc) {}
		virtual ~CPUDescriptorHeap() override = default;

		virtual void CreateBufferShaderResourceView(uint64_t offsetInBytes, IBufferHandle pBuffer, BufferDescriptorDesc const & desc) override;
		virtual void CreateBufferUnorderedAccessView(uint64_t offsetInBytes, IBufferHandle pBuffer, BufferDescriptorDesc const & desc) override;
		virtual void CreateTextureShaderResourceView(uint64_t offsetInBytes, ITextureViewHandle pTextureView) override;
		virtual void CreateTextureUnorderedAccessView(uint64_t offsetInBytes, ITextureViewHandle pTextureView) override;
	};

	struct GPUDescriptorHeap : DescriptorHeapBase, IGPUDescriptorHeap {
		GPUDescriptorHeap(Device * pDevice, DescriptorHeapDesc const & desc) : DescriptorHeapBase(pDevice, desc, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE), IGPUDescriptorHeap(desc) {}
		virtual ~GPUDescriptorHeap() override = default;
	};

	struct DescriptorSetLayout : IDescriptorSetLayout {
		DescriptorSetLayout(DescriptorSetLayoutDesc const & desc) : IDescriptorSetLayout(desc) {}
		virtual ~DescriptorSetLayout() override = default;
	};

}