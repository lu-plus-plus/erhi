// for optional queue family indices
#include <optional>
// for mandatory primary queue family
#include <stdexcept>
//#include <format>
//#include <iostream>

#include "erhi/vulkan/device.hpp"
#include "erhi/vulkan/physical_device.hpp"



namespace erhi::vk {

	Device::Device(PhysicalDeviceHandle physicalDeviceHandle) :
		IDevice{ physicalDeviceHandle },
		mpPhysicalDevice{ physicalDeviceHandle },
		mDevice{ VK_NULL_HANDLE } {
	
		std::optional<uint32_t> primaryQueueFamilyIndex;
		std::optional<uint32_t> computeQueueFamilyIndex;
		std::optional<uint32_t> copyQueueFamilyIndex;

		constexpr VkQueueFlags primaryQueueFlags{ VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT };
		constexpr VkQueueFlags computeQueueFlags{ VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT };
		constexpr VkQueueFlags copyQueueFlags{ VK_QUEUE_TRANSFER_BIT };

		for (uint32_t queueFamilyIndex = 0; queueFamilyIndex < mpPhysicalDevice->mQueueFamilies.size(); ++queueFamilyIndex) {
			auto const & queueFamilyProperties = mpPhysicalDevice->mQueueFamilies[queueFamilyIndex];

			//std::cout << std::format("queue: {}{}{}{}{}\n",
			//	queueFamilyProperties.queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT ? "graphics, " : "",
			//	queueFamilyProperties.queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT ? "compute, " : "",
			//	queueFamilyProperties.queueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT ? "transfer, " : "",
			//	queueFamilyProperties.queueFamilyProperties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT ? "sparse binding, " : "",
			//	queueFamilyProperties.queueFamilyProperties.queueFlags & VK_QUEUE_PROTECTED_BIT ? "protected." : ""
			//);

			if ((queueFamilyProperties.queueFamilyProperties.queueFlags & primaryQueueFlags) == primaryQueueFlags) {
				primaryQueueFamilyIndex = queueFamilyIndex;
			}
			else if ((queueFamilyProperties.queueFamilyProperties.queueFlags & primaryQueueFlags) == computeQueueFlags) {
				computeQueueFamilyIndex = queueFamilyIndex;
			}
			else if ((queueFamilyProperties.queueFamilyProperties.queueFlags & primaryQueueFlags) == copyQueueFlags) {
				copyQueueFamilyIndex = queueFamilyIndex;
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

		if (not primaryQueueFamilyIndex) {
			throw std::runtime_error("failed to find a primary queue family on device '" + std::string(mpPhysicalDevice->name()) + "'");
		}
		queueCreateInfos.push_back(GetDeviceQueueCreateInfo(primaryQueueFamilyIndex.value()));

		if (computeQueueFamilyIndex) {
			queueCreateInfos.push_back(GetDeviceQueueCreateInfo(computeQueueFamilyIndex.value()));
		}

		if (copyQueueFamilyIndex) {
			queueCreateInfos.push_back(GetDeviceQueueCreateInfo(copyQueueFamilyIndex.value()));
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

		vkCheckResult(vkCreateDevice(mpPhysicalDevice->mPhysicalDevice, &deviceCreateInfo, nullptr, &mDevice));
		
	}

	Device::~Device() {
		vkDestroyDevice(mDevice, nullptr);
	}

}