#ifndef ERHI_DX12_DEVICE_HPP
#define ERHI_DX12_DEVICE_HPP

#include "../../common/context/device.hpp"
#include "../native.hpp"



namespace erhi::dx12 {

	struct Device : IDevice {

		PhysicalDeviceHandle			mPhysicalDeviceHandle;

		ID3D12Device *					mpDevice;
		ID3D12InfoQueue1 *				mpInfoQueue;
		DWORD							mMessageCallbackCookie;

		Device(PhysicalDevice * pPhysicalDevice, ID3D12Device * pDevice);
		virtual ~Device() override;

		virtual IPhysicalDeviceHandle		GetPhysicalDevice() const override;

		virtual IQueueHandle				SelectQueue(QueueType queueType) override;

		virtual IMemoryHandle				AllocateMemory(MemoryDesc const & desc) override;
		virtual MemoryRequirements			GetBufferMemoryRequirements(MemoryHeapType heapType, BufferDesc const & bufferDesc) override;
		virtual IPlacedBufferHandle			CreatePlacedBuffer(IMemoryHandle memoryHandle, uint64_t offset, uint64_t alignment, BufferDesc const & bufferDesc) override;
		virtual ICommittedBufferHandle		CreateCommittedBuffer(MemoryHeapType heapType, BufferDesc const & bufferDesc) override;

	};

}



#endif // ERHI_DX12_DEVICE_HPP