#include "erhi/vulkan/physical_device.hpp"



namespace erhi::vk {

	PhysicalDevice::PhysicalDevice(VkInstance instance, VkPhysicalDevice nativeDevice) :
		IPhysicalDevice{},
		mNativeDevice{ nativeDevice },
		mProperties{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, .pNext = nullptr, .properties = {} },
		mFeatures{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = nullptr, .features = {} },
		mExtensions{},
		mQueueFamilies{} {

		vkGetPhysicalDeviceProperties2(nativeDevice, &mProperties);
		vkGetPhysicalDeviceFeatures2(nativeDevice, &mFeatures);

		uint32_t extensionCount = 0;
		vkCheckResult(vkEnumerateDeviceExtensionProperties(mNativeDevice, nullptr, &extensionCount, nullptr));
		mExtensions.resize(extensionCount);
		vkCheckResult(vkEnumerateDeviceExtensionProperties(mNativeDevice, nullptr, &extensionCount, mExtensions.data()));

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties2(mNativeDevice, &queueFamilyCount, nullptr);
		mQueueFamilies.resize(queueFamilyCount, VkQueueFamilyProperties2{ VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2 });
		vkGetPhysicalDeviceQueueFamilyProperties2(mNativeDevice, &queueFamilyCount, mQueueFamilies.data());

	}

	PhysicalDevice::~PhysicalDevice() = default;

	uint32_t PhysicalDevice::deviceID() const {
		return mProperties.properties.deviceID;
	}

	char const * PhysicalDevice::deviceName() const {
		return mProperties.properties.deviceName;
	}

	PhysicalDeviceType PhysicalDevice::deviceType() const {
		return mProperties.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ? PhysicalDeviceType::Integrated : PhysicalDeviceType::Discrete;
	}

}