#include "erhi/vulkan/device.hpp"
#include "erhi/vulkan/queue.hpp"

namespace erhi::vk {

	Queue::Queue(DeviceHandle deviceHandle, QueueType queueType, VkQueue queue) : IQueue(queueType), mDeviceHandle(deviceHandle), mQueue(queue) {}

	Queue::~Queue() = default;

}