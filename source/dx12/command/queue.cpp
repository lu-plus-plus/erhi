#include "erhi/dx12/context/device.hpp"
#include "erhi/dx12/command/queue.hpp"



namespace erhi::dx12 {

	D3D12_COMMAND_LIST_TYPE MapQueueType(QueueType queueType) {
		switch (queueType) {
			case QueueType::Primary:		return D3D12_COMMAND_LIST_TYPE_DIRECT;
			case QueueType::AsyncCompute:	return D3D12_COMMAND_LIST_TYPE_COMPUTE;
			case QueueType::AsyncCopy:		return D3D12_COMMAND_LIST_TYPE_COPY;
		}
		return D3D12_COMMAND_LIST_TYPE_DIRECT;
	};

	Queue::Queue(DeviceHandle pDevice, QueueType queueType) : IQueue(queueType), mpCommandQueue(nullptr) {
		D3D12_COMMAND_QUEUE_DESC const queueDesc{
			.Type = MapQueueType(queueType),
			.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = 0u
		};

		D3D12CheckResult(pDevice->mpDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mpCommandQueue)));
	}

	Queue::~Queue() {
		mpCommandQueue.Reset();
	}

}