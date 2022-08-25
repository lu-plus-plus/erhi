#include "erhi/vulkan/physical_device.hpp"
#include "erhi/vulkan/device.hpp"



namespace erhi::vk {

	PhysicalDevice::PhysicalDevice(InstanceHandle pInstance, VkPhysicalDevice nativeDevice) :
		IPhysicalDevice{},
		mpInstance{ pInstance },
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



	char const * PhysicalDevice::name() const {
		return mProperties.properties.deviceName;
	}

	PhysicalDeviceType PhysicalDevice::type() const {
		return mProperties.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ? PhysicalDeviceType::Integrated : PhysicalDeviceType::Discrete;
	}



	IDeviceHandle PhysicalDevice::createDevice() const {
		return nullptr;
	}

}