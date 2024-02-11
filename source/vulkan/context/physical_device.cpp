#include <format>

#include "erhi/vulkan/context/context.hpp"



namespace erhi::vk {

	PhysicalDevice::PhysicalDevice(VkPhysicalDevice physicalDevice) :
		mPhysicalDevice(physicalDevice),
		mProperties(), mDescriptorBufferProperties(),
		mFeatures(), mBufferDeviceAddressFeatures(),
		mMemoryProperties(),
		mExtensions{},
		mQueueFamilies{} {

		mProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		mDescriptorBufferProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_PROPERTIES_EXT;
		NextChain(mProperties).Next(mDescriptorBufferProperties);
		vkGetPhysicalDeviceProperties2(physicalDevice, &mProperties);

		mFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		mBufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
		NextChain(mFeatures).Next(mBufferDeviceAddressFeatures);
		vkGetPhysicalDeviceFeatures2(physicalDevice, &mFeatures);

		mMemoryProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
		vkGetPhysicalDeviceMemoryProperties2(physicalDevice, &mMemoryProperties);

		uint32_t extensionCount = 0;
		vkCheckResult(vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &extensionCount, nullptr));
		mExtensions.resize(extensionCount);
		vkCheckResult(vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &extensionCount, mExtensions.data()));

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties2(mPhysicalDevice, &queueFamilyCount, nullptr);
		mQueueFamilies.resize(queueFamilyCount, VkQueueFamilyProperties2{ VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2, nullptr, {} });
		vkGetPhysicalDeviceQueueFamilyProperties2(mPhysicalDevice, &queueFamilyCount, mQueueFamilies.data());
	}



	PhysicalDevice::operator VkPhysicalDevice() const & {
		return mPhysicalDevice;
	}



	char const * PhysicalDevice::DeviceName() const & {
		return mProperties.properties.deviceName;
	}

	VkPhysicalDeviceType PhysicalDevice::DeviceType() const & {
		return mProperties.properties.deviceType;
	}



	uint64_t PhysicalDevice::DescriptorSizeInBytes(VkDescriptorType type) const & {
		switch (type) {
			case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
				return mDescriptorBufferProperties.uniformBufferDescriptorSize;
			case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
				return mDescriptorBufferProperties.storageBufferDescriptorSize;
			case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
				return mDescriptorBufferProperties.sampledImageDescriptorSize;
			case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
				return mDescriptorBufferProperties.storageImageDescriptorSize;
			case VK_DESCRIPTOR_TYPE_SAMPLER:
				return mDescriptorBufferProperties.samplerDescriptorSize;
			default:
				throw std::invalid_argument("querying descriptor data size for an unsupported type");
				return 0;
		}
	}

}