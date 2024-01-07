#include "erhi/vulkan/context/physical_device.hpp"
#include "erhi/vulkan/context/device.hpp"

#include "erhi/vulkan/command/queue.hpp"

#include <stdexcept>		// for mandatory primary queue family
#include <format>			// for formatting exception log



namespace erhi::vk {

	Queue::Queue(VkDevice device, QueueType queueType, uint32_t queueFamilyIndex) : IQueue(queueType), mQueue(VK_NULL_HANDLE), mQueueFamilyIndex(queueFamilyIndex) {
		VkDeviceQueueInfo2 queueInfo{
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2,
			.pNext = nullptr,
			.flags = 0,
			.queueFamilyIndex = queueFamilyIndex,
			.queueIndex = 0
		};

		vkGetDeviceQueue2(device, &queueInfo, &mQueue);
	}

	Queue::~Queue() = default;



	IQueueHandle Device::SelectQueue(QueueType queueType) {
		switch (queueType) {
			case QueueType::Primary: return mPrimaryQueue.get();
			case QueueType::AsyncCompute: return mAsyncComputeQueue.get();
			case QueueType::AsyncCopy: return mAsyncCopyQueue.get();
		}
		return nullptr;
	}

}