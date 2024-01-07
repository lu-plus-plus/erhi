#pragma once

#include <vector>

#include "../../common/context/device.hpp"
#include "../native.hpp"



namespace erhi::dx12 {

	struct Device : IDevice {
		ComPtr<IDXGIFactoryLatest>			mpDXGIFactory;
		ComPtr<ID3D12DebugLatest>			mpD3D12DebugLayer;

		ComPtr<IDXGIAdapterLatest>			mpDXGIAdapter;

		ComPtr<ID3D12DeviceLatest>			mpDevice;
		ComPtr<ID3D12InfoQueueLatest>		mpInfoQueue;
		DWORD								mMessageCallbackCookie;

		ComPtr<D3D12MA::Allocator>			mpMemoryAllocator;

		std::unique_ptr<Queue> mpPrimaryQueue;
		std::unique_ptr<Queue> mpAsyncComputeQueue;
		std::unique_ptr<Queue> mpAsyncCopyQueue;

		std::vector<UINT>					mLookUpTable_descriptorHandleIncrementSize;

		Device(DeviceDesc const & desc, std::shared_ptr<IMessageCallback> pMessageCallback);
		virtual ~Device() override;

		ID3D12DeviceLatest * operator->() const;

		virtual IQueueHandle				SelectQueue(QueueType queueType) override;

		virtual IBufferHandle				CreateBuffer(MemoryHeapType heapType, BufferDesc const & bufferDesc) override;
		virtual ITextureHandle				CreateTexture(MemoryHeapType heapType, TextureDesc const & textureDesc) override;
	};

}