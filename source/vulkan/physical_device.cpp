#include "erhi/vulkan/instance.hpp"
#include "erhi/vulkan/physical_device.hpp"
#include "erhi/vulkan/device.hpp"



namespace erhi::vk {

	PhysicalDevice::PhysicalDevice(InstanceHandle pInstance, VkPhysicalDevice physicalDevice) :
		IPhysicalDevice{},
		mpInstance{ pInstance },
		mPhysicalDevice{ physicalDevice },
		mProperties{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, .pNext = nullptr, .properties = {} },
		mFeatures{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = nullptr, .features = {} },
		mExtensions{},
		mQueueFamilies{} {

		vkGetPhysicalDeviceProperties2(physicalDevice, &mProperties);
		vkGetPhysicalDeviceFeatures2(physicalDevice, &mFeatures);

		uint32_t extensionCount = 0;
		vkCheckResult(vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &extensionCount, nullptr));
		mExtensions.resize(extensionCount);
		vkCheckResult(vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &extensionCount, mExtensions.data()));

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties2(mPhysicalDevice, &queueFamilyCount, nullptr);
		mQueueFamilies.resize(queueFamilyCount, VkQueueFamilyProperties2{ VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2 });
		vkGetPhysicalDeviceQueueFamilyProperties2(mPhysicalDevice, &queueFamilyCount, mQueueFamilies.data());
	}

	PhysicalDevice::~PhysicalDevice() = default;



	char const * PhysicalDevice::name() const {
		return mProperties.properties.deviceName;
	}

	PhysicalDeviceType PhysicalDevice::type() const {
		return mProperties.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ? PhysicalDeviceType::Integrated : PhysicalDeviceType::Discrete;
	}



	IDeviceHandle PhysicalDevice::createDevice(DeviceDesc const & desc) {
		return MakeHandle<Device>(PhysicalDeviceHandle(this));
	}

}