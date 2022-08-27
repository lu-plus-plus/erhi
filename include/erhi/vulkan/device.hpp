#ifndef ERHI_VULKAN_DEVICE_HPP
#define ERHI_VULKAN_DEVICE_HPP

#include "../common/device.hpp"
#include "native.hpp"
#include "handle.hpp"



namespace erhi::vk {

	struct Device : IDevice {

		PhysicalDeviceHandle mpPhysicalDevice;
		
		VkDevice mDevice;

		Device(DeviceDesc const & desc);

		~Device();

	};

}



#endif // ERHI_VULKAN_DEVICE_HPP