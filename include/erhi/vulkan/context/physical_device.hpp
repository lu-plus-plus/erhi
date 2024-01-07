#pragma once

#include "../../common/context/context.hpp"
#include "../native.hpp"

#include <vector>



namespace erhi::vk {

	struct PhysicalDevice {
		VkPhysicalDevice						mPhysicalDevice;

		VkPhysicalDeviceProperties2				mProperties;
		VkPhysicalDeviceFeatures2				mFeatures;
		VkPhysicalDeviceMemoryProperties2		mMemoryProperties;
		std::vector<VkExtensionProperties>		mExtensions;
		std::vector<VkQueueFamilyProperties2>	mQueueFamilies;

		PhysicalDevice(VkPhysicalDevice physicalDevice);

		operator VkPhysicalDevice() const;

		char const * deviceName() const;
		VkPhysicalDeviceType deviceType() const;
	};

}


