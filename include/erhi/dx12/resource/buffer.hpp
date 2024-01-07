#pragma once

#include "../../common/resource/memory.hpp"
#include "../native.hpp"



namespace erhi::dx12 {

	struct Buffer : IBuffer {
		ComPtr<D3D12MA::Allocation>	mpAllocation;
		ComPtr<ID3D12Resource>		mpResource;

		Buffer(Device * pDevice, MemoryHeapType heapType, BufferDesc const & desc);
		virtual ~Buffer() override;
	};

}