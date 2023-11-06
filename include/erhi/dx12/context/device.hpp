#ifndef ERHI_DX12_DEVICE_HPP
#define ERHI_DX12_DEVICE_HPP

#include "../../common/context/device.hpp"
#include "../native.hpp"
#include "D3D12MemAlloc.h"



namespace erhi::dx12 {

	struct Device : IDevice {
		PhysicalDeviceHandle			mPhysicalDeviceHandle;

		ID3D12Device *					mpDevice;
		ID3D12InfoQueue1 *				mpInfoQueue;
		DWORD							mMessageCallbackCookie;

		D3D12MA::Allocator *			mpMemoryAllocator;

		Device(PhysicalDevice * pPhysicalDevice, ID3D12Device * pDevice);
		virtual ~Device() override;

		virtual IPhysicalDeviceHandle		GetPhysicalDevice() const override;

		virtual IQueueHandle				SelectQueue(QueueType queueType) override;

		virtual IMemoryHandle				AllocateMemory(MemoryRequirements const & requirements) override;

		virtual IBufferHandle				CreateBuffer(MemoryHeapType heapType, BufferDesc const & bufferDesc) override;
		virtual MemoryRequirements			GetBufferMemoryRequirements(MemoryHeapType heapType, BufferDesc const & bufferDesc) override;
		virtual IBufferHandle				CreatePlacedBuffer(IMemoryHandle memory, uint64_t offset, BufferDesc const & bufferDesc) override;

		virtual ITextureHandle				CreateTexture(MemoryHeapType heapType, TextureDesc const & textureDesc) override;
		virtual MemoryRequirements			GetTextureMemoryRequirements(MemoryHeapType heapType, TextureDesc const & textureDesc) override;
		virtual ITextureHandle				CreatePlacedTexture(IMemoryHandle memory, uint64_t offset, TextureDesc const & textureDesc) override;
	};

}



#endif // ERHI_DX12_DEVICE_HPP