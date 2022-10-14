#include "erhi/dx12/physical_device.hpp"
#include "erhi/dx12/device.hpp"
#include "erhi/dx12/queue.hpp"

namespace erhi::dx12 {

	Device::Device(PhysicalDevice * pPhysicalDevice, ID3D12Device * pDevice) : IDevice(), mPhysicalDeviceHandle(pPhysicalDevice), mpDevice(pDevice) {}

	Device::~Device() {
		mpDevice->Release();
	}



	IPhysicalDevice * Device::pPhysicalDevice() const {
		return mPhysicalDeviceHandle.get();
	}



	IQueueHandle Device::selectQueue(QueueType queueType) {
		return MakeHandle<Queue>(this, queueType);
	}

}