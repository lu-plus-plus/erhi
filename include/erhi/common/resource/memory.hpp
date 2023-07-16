#pragma once

#include "../common.hpp"



namespace erhi {

	struct IMemory : IObject {
		
		MemoryDesc mDesc;

		IMemory(MemoryDesc const & desc);
		virtual ~IMemory() override;

		virtual IDeviceHandle GetDevice() const = 0;

		virtual IBufferHandle CreatePlacedBuffer(uint64_t offset, uint64_t actualSize, BufferDesc const & bufferDesc) = 0;
		virtual ITextureHandle CreatePlacedTexture(uint64_t offset, uint64_t actualSize, TextureDesc const & textureDesc) = 0;

	};

	//struct MemoryView {
	//	virtual IMemoryHandle MemoryHandle() const = 0;
	//	virtual uint64_t Offset() const = 0;
	//	virtual uint64_t Size() const = 0;
	//};

	namespace traits {

		template <typename T>
		concept IsMemoryView = requires (T const x) {
			{ x.GetMemoryHandle() } -> std::convertible_to<IMemoryHandle>;
			{ x.GetOffset() } -> std::convertible_to<uint64_t>;
			{ x.GetSize() } -> std::convertible_to<uint64_t>;
		};

	}
	


	struct IBuffer : IObject {
		
		BufferDesc mDesc;

		IBuffer(BufferDesc const & desc);
		virtual ~IBuffer() override;

	};

	template <typename MemoryView>
		requires (traits::IsMemoryView<MemoryView> and std::movable<MemoryView>)
	struct IPlacedBuffer : IBuffer {

		MemoryView mMemoryView;

		IPlacedBuffer(MemoryView && memoryView, BufferDesc const & desc) : IBuffer(desc), mMemoryView(std::move(memoryView)) {}

		virtual ~IPlacedBuffer() override = default;

	};



	struct ITexture : IObject {

		TextureDesc mDesc;

		ITexture(TextureDesc const & desc);
		~ITexture();

	};

	template <traits::IsMemoryView MemoryView>
		requires std::movable<MemoryView>
	struct IPlacedTexture : ITexture {

		MemoryView mMemoryView;

		IPlacedTexture(MemoryView && memoryView, TextureDesc const & desc) : ITexture(desc), mMemoryView(std::move(memoryView)) {}

		virtual ~IPlacedTexture() override = default;

	};

}


