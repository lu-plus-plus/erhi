#ifndef ERHI_MEMORY_HPP
#define ERHI_MEMORY_HPP

#include "../common.hpp"



namespace erhi {

	struct IMemory : IObject {
		
	private:

		MemoryDesc mDesc;

	public:

		IMemory(MemoryDesc const & desc);
		virtual ~IMemory() override;

		virtual IDeviceHandle GetDevice() const = 0;

		virtual MemoryDesc const & GetDesc() const;

	};
	


	struct IBuffer : IObject {

		BufferDesc mDesc;

		IBuffer(BufferDesc const & desc);
		virtual ~IBuffer() override;
		
		virtual bool IsCommittedResource() const = 0;

	};



	struct ICommittedBuffer : IBuffer {

		ICommittedBuffer(BufferDesc const & desc);
		virtual ~ICommittedBuffer() override;

		virtual bool IsCommittedResource() const;

	};



	struct IPlacedBuffer : IBuffer {

		uint64_t mOffsetInMemory;
		uint64_t mAlignment;

		IPlacedBuffer(BufferDesc const & desc, uint64_t offsetInMemory, uint64_t alignment);
		virtual ~IPlacedBuffer() override;

		virtual bool IsCommittedResource() const;

		virtual IMemoryHandle GetMemory() const = 0;

	};

}



#endif // ERHI_MEMORY_HPP