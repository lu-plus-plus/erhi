#include "erhi/dx12/message.hpp"
#include "erhi/dx12/instance.hpp"
#include "erhi/dx12/physical_device.hpp"
#include "erhi/dx12/device.hpp"
#include "erhi/dx12/queue.hpp"

namespace erhi::dx12 {

	Device::Device(PhysicalDevice * pPhysicalDevice, ID3D12Device * pDevice) :
		IDevice(), mPhysicalDeviceHandle(pPhysicalDevice),
		mpDevice(pDevice), mpInfoQueue(nullptr), mMessageCallbackCookie(0u) {

		if (mPhysicalDeviceHandle->mInstanceHandle->mIsDebugEnabled) {
			D3D12CheckResult(pDevice->QueryInterface(IID_PPV_ARGS(&mpInfoQueue)));
			D3D12CheckResult(mpInfoQueue->RegisterMessageCallback(AdaptToMessageCallback, D3D12_MESSAGE_CALLBACK_IGNORE_FILTERS, mPhysicalDeviceHandle->mInstanceHandle->mMessageCallbackHandle.get(), &mMessageCallbackCookie));
		}
	}

	Device::~Device() noexcept {
		if (mPhysicalDeviceHandle->mInstanceHandle->mIsDebugEnabled) {
			try {
				D3D12CheckResult(mpInfoQueue->UnregisterMessageCallback(mMessageCallbackCookie));
			}
			catch (bad_api_call const & e) {
				mPhysicalDeviceHandle->mInstanceHandle->mMessageCallbackHandle->error(e.what());
				std::terminate();
			}
			catch (...) {
				mPhysicalDeviceHandle->mInstanceHandle->mMessageCallbackHandle->error("DX12 backend: Unknown exception is thrown in unregistering message callback.");
				std::terminate();
			}
			mpInfoQueue->Release();
		}

		mpDevice->Release();
	}



	IPhysicalDevice * Device::pPhysicalDevice() const {
		return mPhysicalDeviceHandle.get();
	}

	IQueueHandle Device::selectQueue(QueueType queueType) {
		return MakeHandle<Queue>(this, queueType);
	}



	OnDevice::OnDevice(Device * pDevice) : mDeviceHandle(pDevice) {}

	OnDevice::~OnDevice() = default;

	IDevice * OnDevice::pDevice() const {
		return mDeviceHandle.get();
	}

}