#ifndef ERHI_DX12_DEVICE_HPP
#define ERHI_DX12_DEVICE_HPP

#include "../common/device.hpp"
#include "native.hpp"



namespace erhi::dx12 {

	struct Device : IDevice {

		PhysicalDeviceHandle mPhysicalDeviceHandle;

		ID3D12Device * mpDevice;

		Device(PhysicalDevice * pPhysicalDevice, ID3D12Device * pDevice);
		virtual ~Device() override;

		IPhysicalDevice * pPhysicalDevice() const override;

		virtual IQueueHandle selectQueue(QueueType queueType) override;

	};

}



#endif // ERHI_DX12_DEVICE_HPP