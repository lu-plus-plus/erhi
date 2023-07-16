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

		VkBuffer CreateNativeBuffer(uint64_t offset, uint64_t actualSize, BufferDesc const & desc);
		void DestroyNativeBuffer(VkBuffer buffer);

		VkImage CreateNativeTexture(uint64_t offset, uint64_t actualSize, TextureDesc const & desc);
		void DestroyNativeTexture(VkImage image);

	};



	struct CommittedBuffer : public IBuffer {

		DeviceHandle		mDeviceHandle;
		VkDeviceMemory		mDeviceMemory;
		VkBuffer			mBuffer;

		CommittedBuffer(Device * pDevice, MemoryHeapType heapType, BufferDesc const & desc);
		virtual ~CommittedBuffer() override;

	};



	template <typename MemoryView>
		requires (traits::IsMemoryView<MemoryView> and std::movable<MemoryView>)
	struct PlacedBuffer : public IPlacedBuffer<MemoryView> {
		
		using IPlacedBuffer<MemoryView>::mMemoryView;
		VkBuffer			mBuffer;

		PlacedBuffer(MemoryView && memoryView, BufferDesc const & desc) : IPlacedBuffer<MemoryView>(std::move(memoryView), desc), mBuffer(VK_NULL_HANDLE) {
			mBuffer = dynamic_handle_cast<Memory>(mMemoryView.GetMemoryHandle())->CreateNativeBuffer(mMemoryView.GetOffset(), mMemoryView.GetSize(), desc);
		}

		virtual ~PlacedBuffer() override {
			dynamic_handle_cast<Memory>(mMemoryView.GetMemoryHandle())->DestroyNativeBuffer(mBuffer);
		}

	};



	struct CommittedTexture : public ITexture {

		DeviceHandle		mDeviceHandle;
		VkDeviceMemory		mDeviceMemory;
		VkImage				mImage;

		CommittedTexture(Device * pDevice, MemoryHeapType heapType, TextureDesc const & desc);
		virtual ~CommittedTexture() override;

	};



	template <typename MemoryView>
		requires (traits::IsMemoryView<MemoryView> and std::movable<MemoryView>)
	struct PlacedTexture : public IPlacedTexture<MemoryView> {

		using IPlacedTexture<MemoryView>::mMemoryView;
		VkImage				mImage;

		PlacedTexture(MemoryView && memoryView, TextureDesc const & desc) : IPlacedTexture<MemoryView>(std::move(memoryView), desc), mImage(VK_NULL_HANDLE) {
			mImage = dynamic_handle_cast<Memory>(mMemoryView.GetMemoryHandle())->CreateNativeTexture(mMemoryView.GetOffset(), mMemoryView.GetSize(), desc);
		}

		virtual ~PlacedTexture() override {
			dynamic_handle_cast<Memory>(mMemoryView.GetMemoryHandle())->DestroyNativeTexture(mImage);
		}

	};

}