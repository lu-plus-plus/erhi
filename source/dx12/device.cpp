#include "erhi/dx12/physical_device.hpp"
#include "erhi/dx12/device.hpp"
#include "erhi/dx12/queue.hpp"

namespace erhi::dx12 {

	Device::Device(PhysicalDeviceHandle pPhysicalDevice, ID3D12Device * pDevice) : IDevice(pPhysicalDevice), mpDevice(pDevice) {}

	Device::~Device() {
		mpDevice->Release();
	}

	IQueueHandle Device::selectQueue(QueueType queueType) {
		return MakeHandle<Queue>(this, queueType);
	}

}