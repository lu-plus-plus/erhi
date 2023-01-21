#include "erhi/dx12/context/instance.hpp"
#include "erhi/dx12/context/physical_device.hpp"

namespace erhi::dx12 {

	Instance::Instance(InstanceDesc const & desc) : IInstance(desc), mpDebugLayer(nullptr), mpFactory(nullptr) {
		UINT dxgiFactoryFlags{ 0u };

		if (desc.enableDebug) {
			ID3D12Debug * pDebugLayer;
			D3D12CheckResult(D3D12GetDebugInterface(IID_PPV_ARGS(&pDebugLayer)));
			
			D3D12CheckResult(pDebugLayer->QueryInterface(IID_PPV_ARGS(&mpDebugLayer)));
			mpDebugLayer->EnableDebugLayer();
			mpDebugLayer->SetEnableGPUBasedValidation(true);
			
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
			
			pDebugLayer->Release();
		}

		D3D12CheckResult(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&mpFactory)));
	}

	Instance::~Instance() = default;

	std::vector<IPhysicalDeviceHandle> Instance::listPhysicalDevices() {
		std::vector<IPhysicalDeviceHandle> handles;
		
		for (UINT adapterIndex = 0; ; ++adapterIndex) {
			IDXGIAdapter1 * pAdapter = nullptr;
			if (DXGI_ERROR_NOT_FOUND == mpFactory->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&pAdapter))) break;

			auto pPhysicalDevice = MakeHandle<PhysicalDevice>(this, pAdapter);

			if (S_FALSE == D3D12CreateDevice(pPhysicalDevice->mpAdapter, D3D_FEATURE_LEVEL_12_1, __uuidof(ID3D12Device), nullptr)) {
				handles.push_back(std::move(pPhysicalDevice));
			}
		}
		
		return handles;
	}

	IPhysicalDeviceHandle Instance::selectPhysicalDevice(PhysicalDeviceDesc const & desc) {
		for (UINT adapterIndex = 0; ; ++adapterIndex) {
			IDXGIAdapter1 * pAdapter = nullptr;
			if (DXGI_ERROR_NOT_FOUND == mpFactory->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&pAdapter))) break;

			auto pPhysicalDevice = MakeHandle<PhysicalDevice>(this, pAdapter);

			if (desc.type == pPhysicalDevice->type()) return pPhysicalDevice;
		}
		return nullptr;
	}

	IInstanceHandle createInstance(InstanceDesc const & desc) {
		return MakeHandle<Instance>(desc);
	}

}