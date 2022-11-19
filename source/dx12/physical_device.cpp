#define _CRT_SECURE_NO_WARNINGS

#include "erhi/dx12/instance.hpp"
#include "erhi/dx12/physical_device.hpp"
#include "erhi/dx12/device.hpp"

namespace erhi::dx12 {

	PhysicalDevice::PhysicalDevice(Instance * pInstance, IDXGIAdapter1 * pAdapter) :
		mInstanceHandle(pInstance),
		mpAdapter(pAdapter),
		mDesc(),
		mName(),
		mFeatureArchitecture() {
		
		mpAdapter->GetDesc1(&mDesc);
		std::wcstombs(mName, mDesc.Description, sizeof(mName));

		ID3D12Device * pDevice = createDeviceInternal();
		
		mFeatureArchitecture.NodeIndex = 0u;
		D3D12CheckResult(pDevice->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE1, &mFeatureArchitecture, sizeof(mFeatureArchitecture)));

		pDevice->Release();
	}

	PhysicalDevice::~PhysicalDevice() {
		mpAdapter->Release();
	}



	IInstance * PhysicalDevice::pInstance() const {
		return mInstanceHandle.get();
	}

	char const * PhysicalDevice::name() const {
		return mName;
	}

	PhysicalDeviceType PhysicalDevice::type() const {
		return mFeatureArchitecture.UMA ? PhysicalDeviceType::Integrated : PhysicalDeviceType::Discrete;
	}

	bool PhysicalDevice::isCacheCoherentUMA() const {
		return mFeatureArchitecture.CacheCoherentUMA;
	}



	IDeviceHandle PhysicalDevice::createDevice(DeviceDesc const & desc) {
		return MakeHandle<Device>(this, createDeviceInternal());
	}

	ID3D12Device * PhysicalDevice::createDeviceInternal() const {
		ID3D12Device * pDevice = nullptr;
		D3D12CheckResult(D3D12CreateDevice(mpAdapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&pDevice)));
		return pDevice;
	}

}