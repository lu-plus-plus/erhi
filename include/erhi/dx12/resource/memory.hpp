#ifndef ERHI_DX12_MEMORY_HPP
#define ERHI_DX12_MEMORY_HPP

#include "../../common/resource/memory.hpp"

#include "../native.hpp"

#include "D3D12MemAlloc.h"



namespace erhi::dx12 {

	struct Memory : IMemory {
		DeviceHandle			mDeviceHandle;
		D3D12MA::Allocation *	mpAllocation;

		Memory(Device * pDevice, MemoryRequirements const & requirements);
		virtual ~Memory() override;

		virtual IDeviceHandle GetDevice() const override;
	};



	struct Buffer : IBuffer {
		DeviceHandle			mDeviceHandle;
		D3D12MA::Allocation *	mpAllocation;
		ID3D12Resource *		mpResource;

		Buffer(Device * pDevice, MemoryHeapType heapType, BufferDesc const & desc);
		virtual ~Buffer() override;
	};



	struct PlacedBuffer : IBuffer {
		MemoryHandle			mMemoryHandle;
		ID3D12Resource *		mpBuffer;

		PlacedBuffer(Memory * pMemory, uint64_t offset, BufferDesc const & desc);
		virtual ~PlacedBuffer() override;
	};



	struct Texture : ITexture {
		DeviceHandle			mDeviceHandle;
		D3D12MA::Allocation *	mpAllocation;
		ID3D12Resource *		mpResource;

		Texture(Device * pDevice, MemoryHeapType heapType, TextureDesc const & desc);
		virtual ~Texture() override;
	};



	struct PlacedTexture : ITexture {
		MemoryHandle			mMemoryHandle;
		ID3D12Resource *		mpTexture;

		PlacedTexture(Memory * pMemory, uint64_t offset, TextureDesc const & desc);
		virtual ~PlacedTexture() override;
	};

}



#endif // ERHI_DX12_MEMORY_HPP