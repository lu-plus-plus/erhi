#include <format>

#include "erhi/vulkan/context/context.hpp"



namespace erhi::vk {

	PhysicalDevice::PhysicalDevice(VkPhysicalDevice physicalDevice) :
		mPhysicalDevice(physicalDevice),
		mProperties{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, .pNext = nullptr, .properties = {} },
		mFeatures{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = nullptr, .features = {} },
		mMemoryProperties{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2, .pNext = nullptr, .memoryProperties = {} },
		mExtensions{},
		mQueueFamilies{} {

		vkGetPhysicalDeviceProperties2(physicalDevice, &mProperties);
		vkGetPhysicalDeviceFeatures2(physicalDevice, &mFeatures);
		vkGetPhysicalDeviceMemoryProperties2(physicalDevice, &mMemoryProperties);

		uint32_t extensionCount = 0;
		vkCheckResult(vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &extensionCount, nullptr));
		mExtensions.resize(extensionCount);
		vkCheckResult(vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &extensionCount, mExtensions.data()));

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties2(mPhysicalDevice, &queueFamilyCount, nullptr);
		mQueueFamilies.resize(queueFamilyCount, VkQueueFamilyProperties2{ VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2 });
		vkGetPhysicalDeviceQueueFamilyProperties2(mPhysicalDevice, &queueFamilyCount, mQueueFamilies.data());
	}



	PhysicalDevice::operator VkPhysicalDevice() const {
		return mPhysicalDevice;
	}



	char const * PhysicalDevice::deviceName() const {
		return mProperties.properties.deviceName;
	}

	VkPhysicalDeviceType PhysicalDevice::deviceType() const {
		return mProperties.properties.deviceType;
	}

}