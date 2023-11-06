#include "erhi/dx12/context/message.hpp"
#include "erhi/dx12/context/instance.hpp"
#include "erhi/dx12/context/physical_device.hpp"
#include "erhi/dx12/context/device.hpp"



namespace erhi::dx12 {

	Device::Device(PhysicalDevice * pPhysicalDevice, ID3D12Device * pDevice) :
		IDevice(), mPhysicalDeviceHandle(pPhysicalDevice),
		mpDevice(pDevice), mpInfoQueue(nullptr), mMessageCallbackCookie(0u), mpMemoryAllocator(nullptr) {

		if (mPhysicalDeviceHandle->mInstanceHandle->mIsDebugEnabled) {
			D3D12CheckResult(pDevice->QueryInterface(IID_PPV_ARGS(&mpInfoQueue)));
			D3D12CheckResult(mpInfoQueue->RegisterMessageCallback(AdaptToMessageCallback, D3D12_MESSAGE_CALLBACK_IGNORE_FILTERS, mPhysicalDeviceHandle->mInstanceHandle->mMessageCallbackHandle.get(), &mMessageCallbackCookie));
		}

		D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
		allocatorDesc.pDevice = mpDevice;
		allocatorDesc.pAdapter = mPhysicalDeviceHandle->mpAdapter;

		D3D12CheckResult(D3D12MA::CreateAllocator(&allocatorDesc, &mpMemoryAllocator));
	}

	Device::~Device() noexcept {
		mpMemoryAllocator->Release();

		if (mPhysicalDeviceHandle->mInstanceHandle->mIsDebugEnabled) {
			D3D12ExitOnError(mpInfoQueue->UnregisterMessageCallback(mMessageCallbackCookie));
			mpInfoQueue->Release();
		}

		mpDevice->Release();
	}



	IPhysicalDeviceHandle Device::GetPhysicalDevice() const {
		return mPhysicalDeviceHandle;
	}

}