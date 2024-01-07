#include "erhi/dx12/context/context.hpp"
#include "erhi/dx12/command/command.hpp"



namespace erhi::dx12 {

	IDeviceHandle CreateDevice(DeviceDesc const & desc, std::shared_ptr<IMessageCallback> pMessageCallback) {
		return new Device(desc, pMessageCallback);
	}



	Device::Device(DeviceDesc const & desc, std::shared_ptr<IMessageCallback> pMessageCallback) : IDevice(desc, pMessageCallback), mMessageCallbackCookie(0)
	{
		// Create a DXGI factory.

		UINT dxgiFactoryFlags = 0;

		if (desc.enableDebug) {
			ComPtr<ID3D12Debug> pDebugLayer = nullptr;
			D3D12CheckResult(D3D12GetDebugInterface(IID_PPV_ARGS(pDebugLayer.GetAddressOf())));

			D3D12CheckResult(pDebugLayer->QueryInterface(IID_PPV_ARGS(mpD3D12DebugLayer.GetAddressOf())));
			mpD3D12DebugLayer->EnableDebugLayer();
			mpD3D12DebugLayer->SetEnableGPUBasedValidation(true);
			mpD3D12DebugLayer->SetEnableAutoName(true);

			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}

		D3D12CheckResult(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(mpDXGIFactory.GetAddressOf())));

		// Create an adapter, i.e., a physical device,
		// and a D3D12 device on it.

		DXGI_GPU_PREFERENCE const preference = desc.physicalDevicePreference == PhysicalDevicePreference::HighPerformance ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_MINIMUM_POWER;

		for (UINT adapterIndex = 0; ; ++adapterIndex)
		{
			ComPtr<IDXGIAdapterLatest> pAdapter = nullptr;
			ComPtr<ID3D12DeviceLatest> pDevice = nullptr;

			// if there is no more adapter to be enumerated
			if (DXGI_ERROR_NOT_FOUND == mpDXGIFactory->EnumAdapterByGpuPreference(adapterIndex, preference, IID_PPV_ARGS(pAdapter.GetAddressOf())))
			{
				break;
			}

			// Skip warp, the fallback software adapter.
			DXGI_ADAPTER_DESC3 adapterDesc;
			D3D12CheckResult(pAdapter->GetDesc3(&adapterDesc));
			if (adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)
			{
				continue;
			}

			// if a D3D12 device may be created on this adapter
			if (S_FALSE == D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_12_1, __uuidof(ID3D12Device), nullptr))
			{
				// then create a device.
				D3D12CheckResult(D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(pDevice.GetAddressOf())));

				mpDXGIAdapter = std::move(pAdapter);
				mpDevice = std::move(pDevice);
				break;
			}
		}

		if (not mpDXGIAdapter or not mpDevice)
		{
			throw std::runtime_error("There exists no valid DXGIAdapter to create a D3D12 device.");
		}

		// Register the debug message callback.

		if (desc.enableDebug) {
			D3D12CheckResult(mpDevice->QueryInterface(IID_PPV_ARGS(mpInfoQueue.GetAddressOf())));
			D3D12CheckResult(mpInfoQueue->RegisterMessageCallback(MapD3D12DebugMessage, D3D12_MESSAGE_CALLBACK_IGNORE_FILTERS, mpMessageCallback.get(), &mMessageCallbackCookie));
		}

		// Create a default memory allocator.

		D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
		allocatorDesc.pDevice = mpDevice.Get();
		allocatorDesc.pAdapter = mpDXGIAdapter.Get();

		D3D12CheckResult(D3D12MA::CreateAllocator(&allocatorDesc, mpMemoryAllocator.GetAddressOf()));

		// Create command queues.

		mpPrimaryQueue = std::make_unique<Queue>(this, QueueType::Primary);
		mpAsyncComputeQueue = std::make_unique<Queue>(this, QueueType::AsyncCompute);
		mpAsyncCopyQueue = std::make_unique<Queue>(this, QueueType::AsyncCopy);

		// Look-up tables for device-specific constants.

		mLookUpTable_descriptorHandleIncrementSize.resize(D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES);
		for (size_t i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
		{
			UINT const value = mpDevice->GetDescriptorHandleIncrementSize(static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i));
			mLookUpTable_descriptorHandleIncrementSize[i] = value;
		}
	}

	Device::~Device() {
		// Though all the ComPtrs will be released correctly, the order of their destruction is not guaranteed.

		mpPrimaryQueue.reset();
		mpAsyncComputeQueue.reset();
		mpAsyncCopyQueue.reset();

		mpMemoryAllocator.Reset();

		if (mMessageCallbackCookie != 0) {
			D3D12ExitOnError(mpInfoQueue->UnregisterMessageCallback(mMessageCallbackCookie));
		}
		mpInfoQueue.Reset();

		mpDevice.Reset();
		mpDXGIAdapter.Reset();

		mpDXGIFactory.Reset();
		mpD3D12DebugLayer.Reset();

		mpMessageCallback.reset();
	}

	Device::operator ID3D12DeviceLatest &() const
	{
		return *mpDevice.Get();
	}

}