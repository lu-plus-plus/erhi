#ifndef ERHI_DX12_MEMORY_HPP
#define ERHI_DX12_MEMORY_HPP

#include "../../common/resource/memory.hpp"
#include "../native.hpp"



namespace erhi::dx12 {

	struct Memory : IMemory {
		ID3D12Heap1 * mpHeap;

		Memory(Device * pDevice, uint32_t size, MemoryLocation location, MemoryHostAccess hostAccess);
		virtual ~Memory() override;
	};

}



#endif // ERHI_DX12_MEMORY_HPP