#include "erhi/dx12/context/context.hpp"
#include "erhi/dx12/resource/resource.hpp"

namespace erhi::dx12 {

	D3D12_HEAP_TYPE MapMemoryHeapType(MemoryHeapType heapType) {
		switch (heapType) {
			case erhi::MemoryHeapType::Default:
				return D3D12_HEAP_TYPE_DEFAULT;
				break;
			case erhi::MemoryHeapType::Upload:
				return D3D12_HEAP_TYPE_UPLOAD;
				break;
			case erhi::MemoryHeapType::ReadBack:
			default:
				return D3D12_HEAP_TYPE_READBACK;
				break;
		}
	}

}