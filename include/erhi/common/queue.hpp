#ifndef ERHI_QUEUE_HPP
#define ERHI_QUEUE_HPP

#include "erhi/common/object.hpp"



namespace erhi {

	enum class QueueType : uint32_t {
		Primary,
		Compute,
		Copy
	};

	struct IQueue : IObject {
		QueueType mQueueType;

		IQueue(QueueType queueType);
		~IQueue();
	};

}



#endif // ERHI_QUEUE_HPP