#include "erhi/common/command/queue.hpp"

namespace erhi {

	IQueue::IQueue(QueueType queueType) : mQueueType(queueType) {}

	IQueue::~IQueue() = default;

}