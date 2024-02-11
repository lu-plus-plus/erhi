#pragma once

#include "../../common/context/device.hpp"
#include "../context/physical_device.hpp"
#include "../native.hpp"



namespace erhi::vk {

	struct Device : IDevice {
		VkInstance mInstance;
		VkDebugUtilsMessengerEXT mDebugUtilsMessenger;
		std::unique_ptr<PhysicalDevice> mpPhysicalDevice;
		VkDevice mDevice;
		
		uint32_t mGraphicsQueueFamilyIndex;
		uint32_t mComputeQueueFamilyIndex;
		uint32_t mCopyQueueFamilyIndex;

		std::unique_ptr<Queue> mPrimaryQueue;
		std::unique_ptr<Queue> mAsyncComputeQueue;
		std::unique_ptr<Queue> mAsyncCopyQueue;

		VmaAllocator	mAllocator;

		Device(DeviceDesc const & desc, std::shared_ptr<IMessageCallback> pMessageCallback);
		virtual ~Device() override;

		operator VkDevice() const;

		virtual IQueueHandle		SelectQueue(QueueType queueType) override;
		virtual ICommandPoolHandle	CreateCommandPool(CommandPoolDesc const & desc) override;

		virtual IBufferHandle		CreateBuffer(MemoryHeapType heapType, BufferDesc const & bufferDesc) override;
		virtual ITextureHandle		CreateTexture(MemoryHeapType heapType, TextureDesc const & textureDesc) override;

		virtual ITextureViewHandle			CreateTextureView(ITextureHandle pTexture, TextureViewDesc const & desc) override;

		virtual ICPUDescriptorHeapHandle	CreateCPUDescriptorHeap(DescriptorHeapDesc const & desc) override;
		virtual IGPUDescriptorHeapHandle	CreateGPUDescriptorHeap(DescriptorHeapDesc const & desc) override;
		virtual IDescriptorSetLayoutHandle	CreateDescriptorSetLayout(DescriptorSetLayoutDesc const & desc) override;
		virtual uint64_t					GetDescriptorSetLayoutSize(IDescriptorSetLayoutHandle pLayout) override;
		virtual uint64_t					GetDescriptorSetLayoutBindingOffset(IDescriptorSetLayoutHandle pLayout, uint64_t binding) override;
	};

}


