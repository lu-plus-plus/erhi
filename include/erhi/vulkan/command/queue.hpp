#pragma once

#include "../../common/command/queue.hpp"
#include "../native.hpp"



namespace erhi::vk {

	struct Queue : IQueue {
		VkQueue			mQueue;
		uint32_t		mQueueFamilyIndex;

		Queue(VkDevice device, QueueType queueType, uint32_t queueFamilyIndex);
		virtual ~Queue() override;
	};

}


