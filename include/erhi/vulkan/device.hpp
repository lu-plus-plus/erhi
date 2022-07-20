#ifndef ERHI_VULKAN_DEVICE_HPP
#define ERHI_VULKAN_DEVICE_HPP

#include "../common/device.hpp"



namespace erhi::vk {

	class Device : IDevice {

		std::unique_ptr<MessageCallback> debugMessageCallback;

		Device(DeviceDesc && desc);

	};

}



#endif // ERHI_VULKAN_DEVICE_HPP