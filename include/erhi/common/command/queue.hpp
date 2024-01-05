#pragma once

#include "../common.hpp"



namespace erhi {

	struct IQueue {
		QueueType mQueueType;

		IQueue(QueueType queueType);
		virtual ~IQueue() = 0;
	};

}