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

			//std::cout << std::format("queue: {}{}{}{}{}\n",
			//	queueFamilyProperties.queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT ? "graphics, " : "",
			//	queueFamilyProperties.queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT ? "compute, " : "",
			//	queueFamilyProperties.queueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT ? "transfer, " : "",
			//	queueFamilyProperties.queueFamilyProperties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT ? "sparse binding, " : "",
			//	queueFamilyProperties.queueFamilyProperties.queueFlags & VK_QUEUE_PROTECTED_BIT ? "protected." : ""
			//);

			if ((queueFamilyProperties.queueFamilyProperties.queueFlags & graphicsQueueFlags) == graphicsQueueFlags) {
				mGraphicsQueueFamilyIndex = queueFamilyIndex;
			}
			else if ((queueFamilyProperties.queueFamilyProperties.queueFlags & graphicsQueueFlags) == computeQueueFlags) {
				mComputeQueueFamilyIndex = queueFamilyIndex;
			}
			else if ((queueFamilyProperties.queueFamilyProperties.queueFlags & graphicsQueueFlags) == copyQueueFlags) {
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
	}



	Device::~Device() {
		vkDestroyDevice(mDevice, nullptr);
	}



	Device::operator VkDevice() const {
		return mDevice;
	}



	IPhysicalDevice * Device::pPhysicalDevice() const {
		return mPhysicalDeviceHandle.get();
	}

	IQueueHandle Device::selectQueue(QueueType queueType) {
		auto deviceHandle = DeviceHandle(this);

		switch (queueType) {
			case QueueType::Graphics: {
				return MakeHandle<Queue>(deviceHandle, queueType, mGraphicsQueueFamilyIndex, 0u);
			} break;

			case QueueType::Compute: {
				if (mComputeQueueFamilyIndex) {
					return MakeHandle<Queue>(deviceHandle, queueType, mComputeQueueFamilyIndex.value(), 0u);
				}
				else {
					throw std::runtime_error(std::format("No dedicated compute queue is found on device '{}'.", mPhysicalDeviceHandle->name()));
				}
			} break;

			case QueueType::Copy: {
				if (mCopyQueueFamilyIndex) {
					return MakeHandle<Queue>(deviceHandle, queueType, mCopyQueueFamilyIndex.value(), 0u);
				}
				else {
					throw std::runtime_error(std::format("No dedicated copy queue is found on device '{}'.", mPhysicalDeviceHandle->name()));
				}
			} break;

			default: break;
		}

		return nullptr;
	}



	OnDevice::OnDevice(Device * pDevice) : mDeviceHandle(pDevice) {}

	OnDevice::~OnDevice() = default;

	IDevice * OnDevice::pDevice() const {
		return mDeviceHandle.get();
	}

}