#pragma once

#include "../../common/context/context.hpp"
#include "../native.hpp"

#include <vector>



namespace erhi::vk {

	struct PhysicalDevice {
		VkPhysicalDevice mPhysicalDevice;

		VkPhysicalDeviceProperties2						mProperties;
		VkPhysicalDeviceDescriptorBufferPropertiesEXT	mDescriptorBufferProperties;

		VkPhysicalDeviceFeatures2						mFeatures;
		VkPhysicalDeviceBufferDeviceAddressFeatures		mBufferDeviceAddressFeatures;
		
		VkPhysicalDeviceMemoryProperties2		mMemoryProperties;
		std::vector<VkExtensionProperties>		mExtensions;
		std::vector<VkQueueFamilyProperties2>	mQueueFamilies;

		PhysicalDevice(VkPhysicalDevice physicalDevice);

		operator VkPhysicalDevice() const &;

		char const * DeviceName() const &;
		VkPhysicalDeviceType DeviceType() const &;
		
		uint64_t DescriptorSizeInBytes(VkDescriptorType type) const &;
	};

}


