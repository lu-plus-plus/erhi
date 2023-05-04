#include "erhi/vulkan/context/physical_device.hpp"
#include "erhi/vulkan/context/device.hpp"

#include "erhi/vulkan/command/queue.hpp"

#include <stdexcept>		// for mandatory primary queue family
#include <format>			// for formatting exception log



namespace erhi::vk {

	Queue::Queue(DeviceHandle deviceHandle, QueueType queueType, uint32_t queueFamilyIndex, uint32_t queueIndexInFamily) :
		IQueue(queueType), mDeviceHandle(deviceHandle), mQueue(VK_NULL_HANDLE) {
	
		VkDeviceQueueInfo2 queueInfo{
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2,
			.pNext = nullptr,
			.flags = 0,
			.queueFamilyIndex = queueFamilyIndex,
			.queueIndex = 0
		};

		vkGetDeviceQueue2(*mDeviceHandle, &queueInfo, &mQueue);
	}

	Queue::~Queue() = default;

	IDevice * Queue::pDevice() const {
		return mDeviceHandle.get();
	}



	IQueueHandle Device::SelectQueue(QueueType queueType) {
		auto deviceHandle = DeviceHandle(this);

		switch (queueType) {
			case QueueType::Primary: {
				return MakeHandle<Queue>(deviceHandle, queueType, mGraphicsQueueFamilyIndex, 0u);
			} break;

			case QueueType::AsyncCompute: {
				if (mComputeQueueFamilyIndex) {
					return MakeHandle<Queue>(deviceHandle, queueType, mComputeQueueFamilyIndex.value(), 0u);
				}
				else {
					throw std::runtime_error(std::format("No dedicated compute queue is found on device '{}'.", mPhysicalDeviceHandle->name()));
				}
			} break;

			case QueueType::AsyncCopy: {
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

}