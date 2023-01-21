#include "erhi/common/command/queue.hpp"

namespace erhi {

	IQueue::IQueue(QueueType queueType) : IObject(), mQueueType(queueType) {}

	IQueue::~IQueue() = default;

}