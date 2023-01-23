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

		virtual IDeviceHandle	GetDevice() const override;
		virtual uint64_t		Size() const override;

	};

}



#endif // ERHI_DX12_MEMORY_HPP