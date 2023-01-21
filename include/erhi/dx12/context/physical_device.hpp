#ifndef ERHI_DX12_PHYSICAL_DEVICE_HPP
#define ERHI_DX12_PHYSICAL_DEVICE_HPP

#include "../../common/context/physical_device.hpp"
#include "../native.hpp"



namespace erhi::dx12 {

	struct PhysicalDevice : IPhysicalDevice {

		InstanceHandle							mInstanceHandle;

		IDXGIAdapter1 *							mpAdapter;
		DXGI_ADAPTER_DESC1						mDesc;
		char									mName[sizeof(DXGI_ADAPTER_DESC1::Description)];
		D3D12_FEATURE_DATA_ARCHITECTURE1		mFeatureArchitecture;
		D3D12_FEATURE_DATA_D3D12_OPTIONS		mFeatureD3D12Options;

		PhysicalDevice(Instance * pInstance, IDXGIAdapter1 * pAdapter);
		virtual ~PhysicalDevice() override;

		virtual IInstance *			pInstance() const override;
		virtual char const *		name() const override;
		virtual PhysicalDeviceType	type() const override;
		virtual bool				isCacheCoherentUMA() const override;
		virtual IDeviceHandle		createDevice(DeviceDesc const & desc) override;

		ID3D12Device * createDeviceInternal() const;

	};

}



#endif // !ERHI_DX12_PHYSICAL_DEVICE_HPP
