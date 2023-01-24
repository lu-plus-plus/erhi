#ifndef ERHI_MEMORY_HPP
#define ERHI_MEMORY_HPP

#include "../common.hpp"



namespace erhi {

	struct IMemory : IObject {
		
	private:

		uint64_t			mSize;

	public:

		IMemory(uint64_t size);
		virtual ~IMemory() override;

		virtual uint64_t			GetSize() const;

		virtual IDeviceHandle		GetDevice() const = 0;

	};

	struct IBuffer : IObject {

	private:

		uint64_t		mOffsetInMemory;
		uint64_t		mSize;

	public:

		IBuffer(uint64_t offsetInMemory, uint64_t size);
		virtual ~IBuffer() override;

		virtual uint64_t		OffsetInMemory() const;
		virtual uint64_t		Size() const;

		virtual IMemoryHandle	GetMemory() const = 0;

	};

}



#endif // ERHI_MEMORY_HPP