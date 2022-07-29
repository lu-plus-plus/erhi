#ifndef ERHI_VULKAN_PHYSICAL_DEVICE_HPP
#define ERHI_VULKAN_PHYSICAL_DEVICE_HPP

#include <vector>

#include "native.hpp"

#include "../common/physical_device.hpp"



namespace erhi::vk {

	struct PhysicalDevice : IPhysicalDevice {
	
	private:

		VkPhysicalDevice						mNativeDevice;
		VkPhysicalDeviceProperties2				mProperties;
		VkPhysicalDeviceFeatures2				mFeatures;
		std::vector<VkExtensionProperties>		mExtensions;
		std::vector<VkQueueFamilyProperties2>	mQueueFamilies;
		
	public:

		PhysicalDevice(VkInstance instance, VkPhysicalDevice nativeDevice);

		~PhysicalDevice();

		virtual uint32_t deviceID() const override;
		virtual char const * deviceName() const override;
		virtual PhysicalDeviceType deviceType() const override;

	};

}



#endif // ERHI_VULKAN_PHYSICAL_DEVICE_HPP