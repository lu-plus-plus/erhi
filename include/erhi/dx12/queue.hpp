#ifndef ERHI_DX12_QUEUE_HPP
#define ERHI_DX12_QUEUE_HPP

#include "../common/queue.hpp"
#include "native.hpp"

namespace erhi::dx12 {

	struct Queue : IQueue {
		DeviceHandle mDeviceHandle;
		ID3D12CommandQueue * mpCommandQueue;

		Queue(DeviceHandle pDevice, QueueType queueType);
		virtual ~Queue() override;

		virtual IDevice * pDevice() const override;
	};

}

#endif // ERHI_DX12_QUEUE_HPP