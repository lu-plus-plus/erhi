#pragma once

#include "../../common/resource/memory.hpp"
#include "../native.hpp"



namespace erhi::vk {

	struct Memory : IMemory {
		DeviceHandle		mDeviceHandle;
		VkDeviceMemory		mMemory;

		Memory(DeviceHandle deviceHandle, MemoryDesc const & desc);
		virtual ~Memory() override;

		virtual IDeviceHandle GetDevice() const override;

		virtual IBufferHandle CreatePlacedBuffer(uint64_t offset, uint64_t actualSize, BufferDesc const & bufferDesc) override;
		virtual ITextureHandle CreatePlacedTexture(uint64_t offset, uint64_t actualSize, TextureDesc const & textureDesc) override;
	};



	struct CommittedBuffer : IBuffer {

		DeviceHandle		mDeviceHandle;
		VkDeviceMemory		mDeviceMemory;
		VkBuffer			mBuffer;

		CommittedBuffer(Device * pDevice, MemoryHeapType heapType, BufferDesc const & desc);
		virtual ~CommittedBuffer() override;

	};

	struct PlacedBuffer : IBuffer {

		MemoryHandle		mMemoryHandle;
		uint64_t			mOffset;
		uint64_t			mActualSize;
		VkBuffer			mBuffer;

		PlacedBuffer(MemoryHandle memoryHandle, uint64_t offset, uint64_t actualSize, BufferDesc const & desc);
		virtual ~PlacedBuffer() override;

	};



	struct CommittedTexture : ITexture {

		DeviceHandle		mDeviceHandle;
		VkDeviceMemory		mDeviceMemory;
		VkImage				mImage;

		CommittedTexture(Device * pDevice, MemoryHeapType heapType, TextureDesc const & desc);
		virtual ~CommittedTexture() override;

	};

	struct PlacedTexture : ITexture {

		MemoryHandle		mMemoryHandle;
		uint64_t			mOffset;
		uint64_t			mActualSize;
		VkImage				mImage;

		PlacedTexture(Memory * pMemory, uint64_t offset, uint64_t actualSize, TextureDesc const & textureDesc);
		virtual ~PlacedTexture() override;

	};

}