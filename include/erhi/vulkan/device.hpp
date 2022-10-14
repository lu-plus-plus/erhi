#ifndef ERHI_VULKAN_DEVICE_HPP
#define ERHI_VULKAN_DEVICE_HPP

// for optional queue family indices
#include <optional>

#include "../common/device.hpp"
#include "native.hpp"



namespace erhi::vk {

	struct Device : IDevice {

		PhysicalDeviceHandle		mPhysicalDeviceHandle;
		VkDevice					mDevice;
		VkQueue						mPrimaryQueue;
		std::optional<VkQueue>		mComputeQueue;
		std::optional<VkQueue>		mCopyQueue;

		Device(PhysicalDevice * pPhysicalDevice);
		~Device();

		operator VkDevice() const;

		virtual IPhysicalDevice * pPhysicalDevice() const override;

		virtual IQueueHandle selectQueue(QueueType queueType) override;

	};

}



#endif // ERHI_VULKAN_DEVICE_HPP