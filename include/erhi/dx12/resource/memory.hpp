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

		virtual IBufferHandle CreatePlacedBuffer(uint64_t offset, uint64_t actualSize, BufferDesc const & bufferDesc) override;
		virtual ITextureHandle CreatePlacedTexture(uint64_t offset, uint64_t actualSize, TextureDesc const & textureDesc) override;

		ID3D12Resource * CreateNativeBuffer(uint64_t offset, uint64_t actualSize, BufferDesc const & bufferDesc);
		void DestroyNativeBuffer(ID3D12Resource * pBuffer);
		
		ID3D12Resource * CreateNativeTexture(uint64_t offset, uint64_t actualSize, TextureDesc const & textureDesc);
		void DestroyNativeTexture(ID3D12Resource * pBuffer);
	};


	struct CommittedBuffer : IBuffer {
		DeviceHandle		mDeviceHandle;
		ID3D12Resource *	mpBuffer;

		CommittedBuffer(Device * pDevice, MemoryHeapType heapType, BufferDesc const & desc);
		virtual ~CommittedBuffer() override;
	};



	template <typename MemoryView>
		requires (traits::IsMemoryView<MemoryView> and std::movable<MemoryView>)
	struct PlacedBuffer : IPlacedBuffer<MemoryView> {
		using IPlacedBuffer<MemoryView>::mMemoryView;
		ID3D12Resource *	mpBuffer;

		PlacedBuffer(MemoryView && memoryView, BufferDesc const & desc) : IPlacedBuffer<MemoryView>(std::move(memoryView), desc), mpBuffer(nullptr) {
			mpBuffer = dynamic_handle_cast<Memory>(mMemoryView.GetMemoryHandle())->CreateNativeBuffer(mMemoryView.GetOffset(), mMemoryView.GetSize(), desc);
		}

		virtual ~PlacedBuffer() override {
			dynamic_handle_cast<Memory>(mMemoryView.GetMemoryHandle())->DestroyNativeBuffer(mpBuffer);
		}
	};



	struct CommittedTexture : ITexture {
		DeviceHandle		mDeviceHandle;
		ID3D12Resource *	mpTexture;

		CommittedTexture(Device * pDevice, MemoryHeapType heapType, TextureDesc const & desc);
		virtual ~CommittedTexture() override;
	};



	template <typename MemoryView>
		requires (traits::IsMemoryView<MemoryView> and std::movable<MemoryView>)
	struct PlacedTexture : IPlacedTexture<MemoryView> {
		using IPlacedTexture<MemoryView>::mMemoryView;
		ID3D12Resource *	mpTexture;

		PlacedTexture(MemoryView && memoryView, TextureDesc const & desc) : IPlacedTexture<MemoryView>(std::move(memoryView), desc), mpTexture(nullptr) {
			mpTexture = dynamic_handle_cast<Memory>(mMemoryView.GetMemoryHandle())->CreateNativeTexture(mMemoryView.GetOffset(), mMemoryView.GetSize(), desc);
		}

		virtual ~PlacedTexture() override {
			dynamic_handle_cast<Memory>(mMemoryView.GetMemoryHandle())->DestroyNativeTexture(mpTexture);
		}
	};

}



#endif // ERHI_DX12_MEMORY_HPP