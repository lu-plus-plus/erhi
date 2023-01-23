#include "erhi/dx12/context/device.hpp"
#include "erhi/dx12/command/queue.hpp"



namespace erhi::dx12 {

	Queue::Queue(DeviceHandle pDevice, QueueType queueType) :
		IQueue(queueType),
		mDeviceHandle(pDevice),
		mpCommandQueue(nullptr) {

		auto ConvertQueueType = [](QueueType queueType) -> D3D12_COMMAND_LIST_TYPE {
			switch (queueType) {
			case QueueType::Graphics:	return D3D12_COMMAND_LIST_TYPE_DIRECT;
			case QueueType::Compute:	return D3D12_COMMAND_LIST_TYPE_COMPUTE;
			case QueueType::Copy:		return D3D12_COMMAND_LIST_TYPE_COPY;
			default:					return D3D12_COMMAND_LIST_TYPE_DIRECT;
			}
		};

		D3D12_COMMAND_QUEUE_DESC queueDesc{
			.Type = ConvertQueueType(queueType),
			.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = 0u
		};
		D3D12CheckResult(mDeviceHandle->mpDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mpCommandQueue)));
	}

	Queue::~Queue() {
		mpCommandQueue->Release();
	}



	IDevice * Queue::pDevice() const {
		return mDeviceHandle.get();
	}



	IQueueHandle Device::SelectQueue(QueueType queueType) {
		return MakeHandle<Queue>(this, queueType);
	}

}