#include "erhi/vulkan/queue.hpp"
#include "erhi/vulkan/device.hpp"

namespace erhi::vk {

	Queue::Queue(DeviceHandle deviceHandle, QueueType queueType, uint32_t queueFamilyIndex) : IQueue(queueType), mDeviceHandle(deviceHandle) {
		VkDeviceQueueInfo2 queueInfo{
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2,
			.pNext = nullptr,
			.flags = 0,
			.queueFamilyIndex = queueFamilyIndex,
			.queueIndex = 0
		};
		vkGetDeviceQueue2(mDeviceHandle->mDevice, &queueInfo, &mQueue);
	}

	Queue::~Queue() = default;

}