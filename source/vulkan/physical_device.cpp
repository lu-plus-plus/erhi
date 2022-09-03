#include "erhi/vulkan/message.hpp"
#include "erhi/vulkan/instance.hpp"
#include "erhi/vulkan/physical_device.hpp"
#include "erhi/vulkan/device.hpp"

#include <format>



namespace erhi::vk {

	PhysicalDevice::PhysicalDevice(InstanceHandle pInstance, VkPhysicalDevice physicalDevice) :
		IPhysicalDevice{},
		mpInstance{ pInstance },
		mPhysicalDevice{ physicalDevice },
		mProperties{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, .pNext = nullptr, .properties = {} },
		mFeatures{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = nullptr, .features = {} },
		mMemoryProperties{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2, .pNext = nullptr, .memoryProperties = {} },
		mExtensions{},
		mQueueFamilies{} {

		vkGetPhysicalDeviceProperties2(physicalDevice, &mProperties);
		vkGetPhysicalDeviceFeatures2(physicalDevice, &mFeatures);
		vkGetPhysicalDeviceMemoryProperties2(physicalDevice, &mMemoryProperties);

		mpInstance->mpMessageCallback->info(mProperties.properties.deviceName);

		for (uint32_t iHeap = 0; iHeap < mMemoryProperties.memoryProperties.memoryHeapCount; ++iHeap) {
			VkMemoryHeap heap{ mMemoryProperties.memoryProperties.memoryHeaps[iHeap] };
			std::string output{ std::format("heap {} - {} MB\n", iHeap, float(heap.size) / 1024.0f / 1024.0f) };
			
			for (uint32_t jType = 0; jType < mMemoryProperties.memoryProperties.memoryTypeCount; ++jType) {
				VkMemoryType memoryType = mMemoryProperties.memoryProperties.memoryTypes[jType];
				if (memoryType.heapIndex != iHeap) continue;

				output += std::format(
					"\ttype {} - {}{}{}{}\n", jType,
					(memoryType.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) ? "device_local " : "",
					(memoryType.propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ? "host_visible " : "",
					(memoryType.propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) ? "host_coherent " : "",
					(memoryType.propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) ? "host_cached " : ""
					//(memoryType.propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) ? "lazily_allocated " : "",
					//(memoryType.propertyFlags & VK_MEMORY_PROPERTY_PROTECTED_BIT) ? "protected " : "",
					//(memoryType.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD) ? "device_coherent " : "",
					//(memoryType.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD) ? "device_uncached " : "",
					//(memoryType.propertyFlags & VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV) ? "rdma_capable " : ""
				);
			}

			output.pop_back();
			mpInstance->mpMessageCallback->info(output);
		}

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