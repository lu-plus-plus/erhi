#include "erhi/vulkan/context/instance.hpp"
#include "erhi/vulkan/context/physical_device.hpp"
#include "erhi/vulkan/context/device.hpp"

#include <format>		// for formatting exception log



namespace erhi::vk {

	Device::Device(PhysicalDevice * pPhysicalDevice) :
		IDevice{},
		mPhysicalDeviceHandle{ pPhysicalDevice },
		mDevice{ VK_NULL_HANDLE },
		mGraphicsQueueFamilyIndex{ std::numeric_limits<uint32_t>::max() },
		mComputeQueueFamilyIndex{},
		mCopyQueueFamilyIndex{} {
	
		constexpr VkQueueFlags graphicsQueueFlags{ VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT };
		constexpr VkQueueFlags computeQueueFlags{ VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT };
		constexpr VkQueueFlags copyQueueFlags{ VK_QUEUE_TRANSFER_BIT };

		for (uint32_t queueFamilyIndex = 0; queueFamilyIndex < mPhysicalDeviceHandle->mQueueFamilies.size(); ++queueFamilyIndex) {
			auto const & queueFamilyProperties = mPhysicalDeviceHandle->mQueueFamilies[queueFamilyIndex];

			if ((queueFamilyProperties.queueFamilyProperties.queueFlags & graphicsQueueFlags) == graphicsQueueFlags) {
				mGraphicsQueueFamilyIndex = queueFamilyIndex;
			}
			else if ((queueFamilyProperties.queueFamilyProperties.queueFlags & computeQueueFlags) == computeQueueFlags) {
				mComputeQueueFamilyIndex = queueFamilyIndex;
			}
			else if ((queueFamilyProperties.queueFamilyProperties.queueFlags & copyQueueFlags) == copyQueueFlags) {
				mCopyQueueFamilyIndex = queueFamilyIndex;
			}
		}

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		auto GetDeviceQueueCreateInfo = [] (uint32_t queueFamilyIndex) -> VkDeviceQueueCreateInfo {
			static float const queuePriority{ 1.0f };
			VkDeviceQueueCreateInfo createInfo{
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.queueFamilyIndex = queueFamilyIndex,
				.queueCount = 1,
				.pQueuePriorities = &queuePriority
			};
			return createInfo;
		};

		if (mGraphicsQueueFamilyIndex == std::numeric_limits<uint32_t>::max()) [[unlikely]] {
			throw std::runtime_error(std::format("Failed to find a graphics queue family on device '{}'.", mPhysicalDeviceHandle->name()));
		}
		queueCreateInfos.push_back(GetDeviceQueueCreateInfo(mGraphicsQueueFamilyIndex));

		if (mComputeQueueFamilyIndex) {
			queueCreateInfos.push_back(GetDeviceQueueCreateInfo(mComputeQueueFamilyIndex.value()));
		}

		if (mCopyQueueFamilyIndex) {
			queueCreateInfos.push_back(GetDeviceQueueCreateInfo(mCopyQueueFamilyIndex.value()));
		}

		VkDeviceCreateInfo deviceCreateInfo{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
			.pQueueCreateInfos = queueCreateInfos.data(),
			.enabledLayerCount = 0,
			.ppEnabledLayerNames = nullptr,
			.enabledExtensionCount = 0,
			.ppEnabledExtensionNames = nullptr,
			.pEnabledFeatures = nullptr
		};

		vkCheckResult(vkCreateDevice(mPhysicalDeviceHandle->mPhysicalDevice, &deviceCreateInfo, nullptr, &mDevice));

		VmaVulkanFunctions vulkanFunctions = {
			.vkGetInstanceProcAddr = vkGetInstanceProcAddr,
			.vkGetDeviceProcAddr = vkGetDeviceProcAddr,
			.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties,
			.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties,
			.vkAllocateMemory = vkAllocateMemory,
			.vkFreeMemory = vkFreeMemory,
			.vkMapMemory = vkMapMemory,
			.vkUnmapMemory = vkUnmapMemory,
			.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges,
			.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges,
			.vkBindBufferMemory = vkBindBufferMemory,
			.vkBindImageMemory = vkBindImageMemory,
			.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements,
			.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements,
			.vkCreateBuffer = vkCreateBuffer,
			.vkDestroyBuffer = vkDestroyBuffer,
			.vkCreateImage = vkCreateImage,
			.vkDestroyImage = vkDestroyImage,
			.vkCmdCopyBuffer = vkCmdCopyBuffer,
			.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2,
			.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2,
			.vkBindBufferMemory2KHR = vkBindBufferMemory2,
			.vkBindImageMemory2KHR = vkBindImageMemory2,
			.vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2,
			.vkGetDeviceBufferMemoryRequirements = vkGetDeviceBufferMemoryRequirements,
			.vkGetDeviceImageMemoryRequirements = vkGetDeviceImageMemoryRequirements
		};
		
		VmaAllocatorCreateInfo allocatorCreateInfo = {};
		allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
		allocatorCreateInfo.physicalDevice = *mPhysicalDeviceHandle;
		allocatorCreateInfo.device = mDevice;
		allocatorCreateInfo.instance = *mPhysicalDeviceHandle->mInstanceHandle;
		allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;

		vkCheckResult(vmaCreateAllocator(&allocatorCreateInfo, &mAllocator));
	}



	Device::~Device() {
		vmaDestroyAllocator(mAllocator);

		vkDestroyDevice(mDevice, nullptr);
	}



	Device::operator VkDevice() const {
		return mDevice;
	}



	IPhysicalDeviceHandle Device::GetPhysicalDevice() const {
		return mPhysicalDeviceHandle;
	}

}