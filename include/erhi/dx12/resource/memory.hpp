#ifndef ERHI_DX12_MEMORY_HPP
#define ERHI_DX12_MEMORY_HPP

#include "../../common/resource/memory.hpp"

#include "../native.hpp"



namespace erhi::dx12 {

	struct Memory : IMemory {
		
		DeviceHandle mDeviceHandle;
		ID3D12Heap * mpHeap;

		Memory(DeviceHandle deviceHandle, D3D12_HEAP_DESC const & heapDesc);
		virtual ~Memory() override;

		virtual IDeviceHandle GetDevice() const override;

	};

	struct Buffer : IBuffer {

		MemoryHandle		mMemoryHandle;
		ID3D12Resource *	mpBuffer;

		Buffer(MemoryHandle memoryHandle, uint64_t offset, BufferDesc const & desc);
		virtual ~Buffer() override;

		virtual IMemoryHandle GetMemory() const override;

	};

}



#endif // ERHI_DX12_MEMORY_HPP