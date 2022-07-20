#include <vulkan/vulkan.h>

#include "erhi/vulkan/device.hpp"
#include "erhi/vulkan/message.hpp"



namespace erhi::vk {

	IDevice * Device::create(DeviceDesc && desc) {
		Device * pDevice{ new Device() };
		pDevice->debugMessageCallback = std::move(desc.debugMessageCallback);



		return pDevice;
	}

	

}