#ifndef ERHI_VULKAN_DEVICE_HPP
#define ERHI_VULKAN_DEVICE_HPP

#include <optional>		// for optional queue family indices

#include "../../common/context/device.hpp"
#include "../native.hpp"

#define VMA_VULKAN_VERSION 1003000
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#include "vk_mem_alloc.h"



namespace erhi::vk {

	struct Device : IDevice {
		PhysicalDeviceHandle		mPhysicalDeviceHandle;

		VkDevice					mDevice;
		
		uint32_t					mGraphicsQueueFamilyIndex;
		std::optional<uint32_t>		mComputeQueueFamilyIndex;
		std::optional<uint32_t>		mCopyQueueFamilyIndex;

		VmaAllocator				mAllocator;

		Device(PhysicalDevice * pPhysicalDevice);
		~Device();

		operator VkDevice() const;

		virtual IPhysicalDeviceHandle		GetPhysicalDevice() const override;

		virtual IQueueHandle				SelectQueue(QueueType queueType) override;

		virtual IMemoryHandle				AllocateMemory(MemoryRequirements const & requirements) override;

		virtual IBufferHandle				CreateBuffer(MemoryHeapType heapType, BufferDesc const & bufferDesc) override;
		virtual MemoryRequirements			GetBufferMemoryRequirements(MemoryHeapType heapType, BufferDesc const & bufferDesc) override;
		virtual IBufferHandle				CreatePlacedBuffer(IMemoryHandle memory, uint64_t offset, BufferDesc const & bufferDesc) override;

		virtual ITextureHandle				CreateTexture(MemoryHeapType heapType, TextureDesc const & textureDesc) override;
		virtual MemoryRequirements			GetTextureMemoryRequirements(MemoryHeapType heapType, TextureDesc const & textureDesc) override;
		virtual ITextureHandle				CreatePlacedTexture(IMemoryHandle memory, uint64_t offset, TextureDesc const & textureDesc) override;
	};

}



#endif // ERHI_VULKAN_DEVICE_HPP