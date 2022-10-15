#include "erhi/vulkan/device.hpp"
#include "erhi/vulkan/queue.hpp"

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

}