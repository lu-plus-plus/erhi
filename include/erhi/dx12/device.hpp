#ifndef ERHI_DX12_DEVICE_HPP
#define ERHI_DX12_DEVICE_HPP

#include "../common/device.hpp"
#include "native.hpp"



namespace erhi::dx12 {

	struct Device : IDevice {

		PhysicalDeviceHandle	mPhysicalDeviceHandle;

		ID3D12Device *			mpDevice;
		ID3D12InfoQueue1 *		mpInfoQueue;
		DWORD					mMessageCallbackCookie;

		Device(PhysicalDevice * pPhysicalDevice, ID3D12Device * pDevice);
		virtual ~Device() override;

		virtual IPhysicalDevice * pPhysicalDevice() const override;

		virtual IQueueHandle selectQueue(QueueType queueType) override;

	};



	struct OnDevice : IOnDevice {
		DeviceHandle mDeviceHandle;

		OnDevice(Device * pDevice);
		virtual ~OnDevice();

		virtual IDevice * pDevice() const;
	};

}



#endif // ERHI_DX12_DEVICE_HPP