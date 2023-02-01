#ifndef ERHI_DX12_MEMORY_HPP
#define ERHI_DX12_MEMORY_HPP

#include "../../common/resource/memory.hpp"

#include "../native.hpp"



namespace erhi::dx12 {

	struct Memory : IMemory {
		
		DeviceHandle mDeviceHandle;
		ID3D12Heap * mpHeap;

		Memory(DeviceHandle deviceHandle, MemoryDesc const & desc);
		virtual ~Memory() override;

		virtual IDeviceHandle GetDevice() const override;

	};

	struct CommittedBuffer : ICommittedBuffer {

		DeviceHandle		mDeviceHandle;
		ID3D12Resource *	mpBuffer;

		CommittedBuffer(Device * pDevice, MemoryHeapType heapType, BufferDesc const & desc);
		virtual ~CommittedBuffer() override;

	};

	struct PlacedBuffer : IPlacedBuffer {

		MemoryHandle		mMemoryHandle;
		ID3D12Resource *	mpBuffer;

		PlacedBuffer(BufferDesc const & desc, Memory * pMemory, uint64_t offset, uint64_t alignment);
		virtual ~PlacedBuffer() override;

		virtual IMemoryHandle GetMemory() const override;

	};

}



#endif // ERHI_DX12_MEMORY_HPP