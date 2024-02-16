#pragma once

#include "../common.hpp"



namespace erhi {

	struct IDevice {
		DeviceDesc mDesc;
		std::shared_ptr<IMessageCallback> mpMessageCallback;
		GlobalConstants mGlobalConstants;

		IDevice(DeviceDesc const & desc, std::shared_ptr<IMessageCallback> pMessageCallback);
		virtual ~IDevice() = 0;

		virtual IMessageCallback &			Log() const;

		virtual IWindowHandle				CreateNewWindow(WindowDesc const & desc) = 0;

		virtual IQueueHandle				SelectQueue(QueueType queueType) = 0;
		virtual ICommandPoolHandle			CreateCommandPool(CommandPoolDesc const & desc) = 0;

		virtual IBufferHandle				CreateBuffer(MemoryHeapType heapType, BufferDesc const & bufferDesc) = 0;
		virtual ITextureHandle				CreateTexture(MemoryHeapType heapType, TextureDesc const & textureDesc) = 0;

		virtual ITextureViewHandle			CreateTextureView(ITextureHandle pTexture, TextureViewDesc const & desc) = 0;

		virtual ICPUDescriptorHeapHandle	CreateCPUDescriptorHeap(DescriptorHeapDesc const & desc) = 0;
		virtual IGPUDescriptorHeapHandle	CreateGPUDescriptorHeap(DescriptorHeapDesc const & desc) = 0;
		virtual IDescriptorSetLayoutHandle	CreateDescriptorSetLayout(DescriptorSetLayoutDesc const & desc) = 0;
		virtual uint64_t					GetDescriptorSetLayoutSize(IDescriptorSetLayoutHandle pLayout) = 0;
		virtual uint64_t					GetDescriptorSetLayoutBindingOffset(IDescriptorSetLayoutHandle pLayout, uint64_t binding) = 0;
	};

	namespace vk {
		IDeviceHandle CreateDevice(DeviceDesc const & desc, std::shared_ptr<IMessageCallback> pMessageCallback);
	}

	namespace dx12 {
		IDeviceHandle CreateDevice(DeviceDesc const & desc, std::shared_ptr<IMessageCallback> pMessageCallback);
	}

}