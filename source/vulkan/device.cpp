// for mandatory primary queue family
#include <stdexcept>
// for formatting exception log
#include <format>
// for debugging
#include <iostream>

#include "erhi/vulkan/physical_device.hpp"
#include "erhi/vulkan/device.hpp"
#include "erhi/vulkan/queue.hpp"



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
		
		auto GetDeviceQueue = [this] (int queueFamilyIndex) {
			VkDeviceQueueInfo2 queueInfo{
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2,
				.pNext = nullptr,
				.flags = 0,
				.queueFamilyIndex = 0,
				.queueIndex = 0
			};
			VkQueue queue;
			vkGetDeviceQueue2(mDevice, &queueInfo, &queue);
			return queue;
		};
		
		mPrimaryQueue = GetDeviceQueue(primaryQueueFamilyIndex.value());
		if (computeQueueFamilyIndex) mComputeQueue = GetDeviceQueue(computeQueueFamilyIndex.value());
		if (copyQueueFamilyIndex) mComputeQueue = GetDeviceQueue(copyQueueFamilyIndex.value());
	}

	Device::~Device() {
		vkDestroyDevice(mDevice, nullptr);
	}

	IQueueHandle Device::selectQueue(QueueType queueType) {
		auto deviceHandle = DeviceHandle(this);

		switch (queueType) {
			case QueueType::Primary: {
				return MakeHandle<Queue>(deviceHandle, queueType, mPrimaryQueue);
			} break;

			case QueueType::Compute: {
				if (mComputeQueue) return MakeHandle<Queue>(deviceHandle, queueType, mComputeQueue.value());
				else throw std::runtime_error(std::format("No compute queue is found on device {}.", mpPhysicalDevice->name()));
			} break;

			case QueueType::Copy: {
				if (mCopyQueue) return MakeHandle<Queue>(deviceHandle, queueType, mCopyQueue.value());
				else throw std::runtime_error(std::format("No copy queue is found on device {}.", mpPhysicalDevice->name()));
			} break;

			default: break;
		}

		return nullptr;
	}

}