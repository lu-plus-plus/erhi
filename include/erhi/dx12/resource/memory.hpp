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

	struct CommittedBuffer : IBuffer {

		DeviceHandle		mDeviceHandle;
		ID3D12Resource *	mpBuffer;

		CommittedBuffer(Device * pDevice, MemoryHeapType heapType, BufferDesc const & desc);
		virtual ~CommittedBuffer() override;

	};

	struct PlacedBuffer : IBuffer {

		Memory *			mMemoryHandle;
		uint64_t			mOffset;
		uint64_t			mActualSize;
		ID3D12Resource *	mpBuffer;

		PlacedBuffer(Memory * pMemory, uint64_t offset, uint64_t actualSize, BufferDesc const & desc);
		virtual ~PlacedBuffer() override;

	};

	struct CommittedTexture : ITexture {
		DeviceHandle		mDeviceHandle;
		ID3D12Resource *	mpTexture;

		CommittedTexture(Device * pDevice, MemoryHeapType heapType, TextureDesc const & desc);
		virtual ~CommittedTexture() override;
	};

	struct PlacedTexture : ITexture {
		MemoryHandle		mMemoryHandle;
		uint64_t			mOffset;
		uint64_t			mActualSize;
		ID3D12Resource *	mpTexture;

		PlacedTexture(Memory * pMemory, uint64_t offset, uint64_t actualSize, TextureDesc const & desc);
		virtual ~PlacedTexture() override;
	};

}



#endif // ERHI_DX12_MEMORY_HPP