#pragma once

#include "../../common/resource/memory.hpp"
#include "../native.hpp"



namespace erhi::vk {

	struct Memory : public IMemory {
		
		DeviceHandle		mDeviceHandle;
		VkDeviceMemory		mMemory;

		Memory(DeviceHandle deviceHandle, MemoryDesc const & desc);
		virtual ~Memory() override;

		virtual IDeviceHandle GetDevice() const override;

		virtual IBufferHandle CreatePlacedBuffer(uint64_t offset, uint64_t actualSize, BufferDesc const & bufferDesc) override;
		virtual ITextureHandle CreatePlacedTexture(uint64_t offset, uint64_t actualSize, TextureDesc const & textureDesc) override;

		struct Slice {
			MemoryHandle mMemoryHandle;
			uint64_t mOffset;
			uint64_t mSize;

			MemoryHandle GetMemoryHandle() const { return mMemoryHandle; }
			uint64_t GetOffset() const { return mOffset; }
			uint64_t GetSize() const { return mSize; }
		};

		VkBuffer CreateNativeBuffer(uint64_t offset, uint64_t actualSize, BufferDesc const & desc);
		void DestroyNativeBuffer(VkBuffer buffer);

	};



	struct CommittedBuffer : public IBuffer {

		DeviceHandle		mDeviceHandle;
		VkDeviceMemory		mDeviceMemory;
		VkBuffer			mBuffer;

		CommittedBuffer(Device * pDevice, MemoryHeapType heapType, BufferDesc const & desc);
		virtual ~CommittedBuffer() override;

	};

	template <typename MemoryView>
	struct PlacedBuffer : public IPlacedBuffer<MemoryView> {
		
		using IPlacedBuffer<MemoryView>::mMemoryView;
		VkBuffer			mBuffer;

		PlacedBuffer(MemoryView && memoryView, BufferDesc const & desc) : IPlacedBuffer<MemoryView>(std::move(memoryView), desc), mBuffer(VK_NULL_HANDLE) {
			mBuffer = mMemoryView.GetMemoryHandle()->CreateNativeBuffer(mMemoryView.GetOffset(), mMemoryView.GetSize(), desc);
		}

		virtual ~PlacedBuffer() override {
			mMemoryView.GetMemoryHandle()->DestroyNativeBuffer(mBuffer);
		}

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