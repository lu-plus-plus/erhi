#ifndef ERHI_DX12_MEMORY_HPP
#define ERHI_DX12_MEMORY_HPP

#include "../../common/resource/memory.hpp"

#include "../native.hpp"



namespace erhi::dx12 {

	struct Memory : IMemory {
		DeviceHandle mDeviceHandle;
		ID3D12Heap1 * mpHeap;

		Memory(Device * pDevice, uint32_t sizeInBytes, MemoryHeapType heapType, BufferUsageFlags bufferUsage, TextureUsageFlags textureUsage);
		virtual ~Memory() override;

		virtual IDevice * pDevice() const override;
	};

}



#endif // ERHI_DX12_MEMORY_HPP