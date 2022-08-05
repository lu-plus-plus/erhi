#ifndef ERHI_VULKAN_PHYSICAL_DEVICE_HPP
#define ERHI_VULKAN_PHYSICAL_DEVICE_HPP

#include <vector>

#include "../common/physical_device.hpp"
#include "native.hpp"



namespace erhi::vk {

	struct PhysicalDevice : IPhysicalDevice {
	
	private:
		
		VkInstance								mInstance;

		VkPhysicalDevice						mNativeDevice;
		VkPhysicalDeviceProperties2				mProperties;
		VkPhysicalDeviceFeatures2				mFeatures;
		std::vector<VkExtensionProperties>		mExtensions;
		std::vector<VkQueueFamilyProperties2>	mQueueFamilies;
		
	public:

		PhysicalDevice(VkInstance instance, VkPhysicalDevice nativeDevice);

		~PhysicalDevice();

		virtual char const * name() const override;
		virtual PhysicalDeviceType type() const override;

		virtual IDeviceHandle createDevice() const override;

	};

}



#endif // ERHI_VULKAN_PHYSICAL_DEVICE_HPP