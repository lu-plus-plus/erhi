#pragma once

#include "../../common/command/queue.hpp"
#include "../native.hpp"

namespace erhi::dx12 {

	D3D12_COMMAND_LIST_TYPE MapQueueType(QueueType queueType);

	struct Queue : IQueue {
		ComPtr<ID3D12CommandQueue> mpCommandQueue;

		Queue(DeviceHandle pDevice, QueueType queueType);
		virtual ~Queue() override;
	};

}