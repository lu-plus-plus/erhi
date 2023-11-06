#pragma once

#include "../../common/resource/memory.hpp"
#include "../native.hpp"



namespace erhi::vk {

	struct Memory : public IMemory {
		DeviceHandle		mDeviceHandle;
		VmaAllocation		mAllocation;

		Memory(DeviceHandle deviceHandle, MemoryRequirements const & requirements);
		virtual ~Memory() override;

		virtual IDeviceHandle GetDevice() const override;
	};



	struct PlacedBuffer : public IBuffer {
		MemoryHandle		mMemoryHandle;
		VkBuffer			mBuffer;

		PlacedBuffer(Memory * pMemory, uint64_t offset, BufferDesc const & desc);
		virtual ~PlacedBuffer() override;
	};



	struct Buffer : public IBuffer {
		DeviceHandle		mDeviceHandle;
		VmaAllocation		mAllocation;
		VkBuffer			mBuffer;

		Buffer(Device * pDevice, MemoryHeapType heapType, BufferDesc const & bufferDesc);
		~Buffer();
	};



	struct PlacedTexture : public ITexture {
		MemoryHandle		mMemoryHandle;
		VkImage				mImage;

		PlacedTexture(Memory * pMemory, uint64_t offset, TextureDesc const & desc);
		virtual ~PlacedTexture() override;
	};



	struct Texture : public ITexture {
		DeviceHandle		mDeviceHandle;
		VmaAllocation		mAllocation;
		VkImage				mImage;

		Texture(Device * pDevice, MemoryHeapType heapType, TextureDesc const & desc);
		virtual ~Texture() override;
	};

}