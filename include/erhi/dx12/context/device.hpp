#pragma once

#include <vector>

#include "../../common/context/device.hpp"
#include "../native.hpp"



namespace erhi::dx12 {

	struct Device : IDevice {
		ComPtr<IDXGIFactoryLatest>			mpDXGIFactory;
		ComPtr<ID3D12DebugLatest>			mpD3D12DebugLayer;

		ComPtr<IDXGIAdapterLatest>			mpDXGIAdapter;

		ComPtr<ID3D12DeviceLatest>			mpDevice;
		ComPtr<ID3D12InfoQueueLatest>		mpInfoQueue;
		DWORD								mMessageCallbackCookie;

		ComPtr<D3D12MA::Allocator>			mpMemoryAllocator;

		std::unique_ptr<Queue>				mpPrimaryQueue;
		std::unique_ptr<Queue>				mpAsyncComputeQueue;
		std::unique_ptr<Queue>				mpAsyncCopyQueue;

		std::vector<UINT>					mLookUpTable_descriptorHandleIncrementSize;
		UINT GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType) const &;

		Device(DeviceDesc const & desc, std::shared_ptr<IMessageCallback> pMessageCallback);
		virtual ~Device() override;

		operator ID3D12DeviceLatest &() const &;

		virtual IWindowHandle				CreateNewWindow(WindowDesc const & desc) override;

		virtual IQueueHandle				SelectQueue(QueueType queueType) override;
		virtual ICommandPoolHandle			CreateCommandPool(CommandPoolDesc const & desc) override;

		virtual IBufferHandle				CreateBuffer(MemoryHeapType heapType, BufferDesc const & bufferDesc) override;
		virtual ITextureHandle				CreateTexture(MemoryHeapType heapType, TextureDesc const & textureDesc) override;

		virtual ITextureViewHandle			CreateTextureView(ITextureHandle pTexture, TextureViewDesc const & desc) override;

		virtual ICPUDescriptorHeapHandle	CreateCPUDescriptorHeap(DescriptorHeapDesc const & desc) override;
		virtual IGPUDescriptorHeapHandle	CreateGPUDescriptorHeap(DescriptorHeapDesc const & desc) override;
		virtual IDescriptorSetLayoutHandle	CreateDescriptorSetLayout(DescriptorSetLayoutDesc const & desc) override;
		virtual uint64_t					GetDescriptorSetLayoutSize(IDescriptorSetLayoutHandle pLayout) override;
		virtual uint64_t					GetDescriptorSetLayoutBindingOffset(IDescriptorSetLayoutHandle pLayout, uint64_t binding) override;

		virtual IRenderPassHandle			CreateRenderPass(RenderPassDesc const & desc) override;
		virtual IFrameBufferHandle			CreateFrameBuffer(FrameBufferDesc const & desc) override;

		virtual IShaderCompilerHandle		CreateShaderCompiler(ShaderCompilerDesc const & desc) override;
	};

}