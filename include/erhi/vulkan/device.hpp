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
		uint32_t					mGraphicsQueueFamilyIndex;
		std::optional<uint32_t>		mComputeQueueFamilyIndex;
		std::optional<uint32_t>		mCopyQueueFamilyIndex;

		Device(PhysicalDevice * pPhysicalDevice);
		~Device();

		operator VkDevice() const;

		virtual IPhysicalDevice *	pPhysicalDevice() const override;
		virtual IQueueHandle		selectQueue(QueueType queueType) override;

	};



	struct OnDevice : IOnDevice {
		DeviceHandle mDeviceHandle;
		
		OnDevice(Device * pDevice);
		virtual ~OnDevice();

		virtual IDevice * pDevice() const;
	};

}



#endif // ERHI_VULKAN_DEVICE_HPP