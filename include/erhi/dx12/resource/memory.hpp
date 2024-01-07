#pragma once

#include "../../common/resource/memory.hpp"
#include "../native.hpp"

namespace erhi::dx12 {

	D3D12_HEAP_TYPE MapMemoryHeapType(MemoryHeapType heapType);

}