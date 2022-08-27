#ifndef ERHI_QUEUE_HPP
#define ERHI_QUEUE_HPP

#include "common.hpp"



namespace erhi {

	struct IQueue : IObject {
		QueueType mQueueType;

		IQueue(QueueType queueType);
		~IQueue();
	};

}



#endif // ERHI_QUEUE_HPP