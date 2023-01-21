#ifndef ERHI_QUEUE_HPP
#define ERHI_QUEUE_HPP

#include "../common.hpp"



namespace erhi {

	struct IQueue : IObject {
		QueueType mQueueType;

		IQueue(QueueType queueType);
		virtual ~IQueue();

		virtual IDevice * pDevice() const = 0;
	};

}



#endif // ERHI_QUEUE_HPP