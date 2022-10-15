#ifndef ERHI_VULKAN_QUEUE_HPP
#define ERHI_VULKAN_QUEUE_HPP

#include "../common/queue.hpp"
#include "native.hpp"



namespace erhi::vk {

	struct Queue : IQueue {
		DeviceHandle	mDeviceHandle;
		VkQueue			mQueue;

		Queue(DeviceHandle deviceHandle, QueueType queueType, uint32_t queueFamilyIndex, uint32_t queueIndexInFamily);
		~Queue();

		virtual IDevice * pDevice() const override;
	};

}



#endif // ERHI_VULKAN_QUEUE_HPP