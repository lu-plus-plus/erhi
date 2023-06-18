#ifndef ERHI_MEMORY_HPP
#define ERHI_MEMORY_HPP

#include "../common.hpp"



namespace erhi {

	struct IMemory : IObject {
		
	protected:

		MemoryDesc mDesc;

	public:

		IMemory(MemoryDesc const & desc);
		virtual ~IMemory() override;

		virtual MemoryDesc const & GetDesc() const;

		virtual IDeviceHandle GetDevice() const = 0;

		virtual IBufferHandle CreatePlacedBuffer(uint64_t offset, uint64_t actualSize, BufferDesc const & bufferDesc) = 0;
		virtual ITextureHandle CreatePlacedTexture(uint64_t offset, uint64_t actualSize, TextureDesc const & textureDesc) = 0;

	};
	


	struct IBuffer : IObject {

	protected:

		BufferDesc mDesc;

	public:

		IBuffer(BufferDesc const & desc);
		virtual ~IBuffer() override;

		virtual BufferDesc const & GetDesc() const;

	};



	struct ITexture : IObject {

	protected:

		TextureDesc mDesc;

	public:

		ITexture(TextureDesc const & desc);
		~ITexture();

		virtual TextureDesc const & GetDesc() const;

	};



	//struct ICommittedBuffer : IBuffer {

	//	ICommittedBuffer(BufferDesc const & desc);
	//	virtual ~ICommittedBuffer() override;

	//	virtual bool IsCommittedResource() const;

	//};



	//struct IPlacedBuffer : IBuffer {

	//	uint64_t mOffsetInMemory;
	//	uint64_t mAlignment;

	//	IPlacedBuffer(BufferDesc const & desc, uint64_t offsetInMemory, uint64_t alignment);
	//	virtual ~IPlacedBuffer() override;

	//	virtual bool IsCommittedResource() const;

	//	virtual IMemoryHandle GetMemory() const = 0;

	//};




	//struct ICommittedTexture : ITexture {

	//	ICommittedTexture(TextureDesc const & desc);
	//	virtual ~ICommittedTexture() override;

	//};

	//struct IPlacedTexture : ITexture {

	//	uint64_t mOffsetInMemory;
	//	uint64_t mAlignment;

	//	IPlacedTexture(TextureDesc const & desc, uint64_t offsetInMemory, uint64_t alignment);
	//	virtual ~IPlacedTexture() override;

	//	virtual IMemoryHandle GetMemory() const = 0;

	//};

}



#endif // ERHI_MEMORY_HPP