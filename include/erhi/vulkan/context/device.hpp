#ifndef ERHI_VULKAN_DEVICE_HPP
#define ERHI_VULKAN_DEVICE_HPP

#include <optional>		// for optional queue family indices

#include "../../common/context/device.hpp"
#include "../native.hpp"



namespace erhi::vk {

	struct Device : IDevice {

		PhysicalDeviceHandle		mPhysicalDeviceHandle;
		VkDevice					mDevice;
		uint32_t					mGraphicsQueueFamilyIndex;
		std::optional<uint32_t>		mComputeQueueFamilyIndex;
		std::optional<uint32_t>		mCopyQueueFamilyIndex;

		Device(PhysicalDevice * pPhysicalDevice);
		~Device();

		operator VkDevice() const;

		virtual IPhysicalDeviceHandle	GetPhysicalDevice() const override;

		virtual IQueueHandle			SelectQueue(QueueType queueType) override;

		virtual IMemoryHandle			AllocateMemory(MemoryDesc const & desc) override;
		virtual MemoryRequirements		GetBufferMemoryRequirements(MemoryHeapType heapType, BufferDesc const & bufferDesc) override;
		//virtual IBufferHandle			CreateBuffer(MemoryHeapType heapType, BufferDesc const & bufferDesc) override;

	};

}



#endif // ERHI_VULKAN_DEVICE_HPP