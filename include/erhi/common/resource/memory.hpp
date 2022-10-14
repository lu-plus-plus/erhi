#ifndef ERHI_MEMORY_HPP
#define ERHI_MEMORY_HPP

#include "../object.hpp"
#include "../handle.hpp"



namespace erhi {

	enum class MemoryHostPaging : uint32_t {
		Unknown,
		NotAvailable,
		WriteCombine,
		WriteBack
	};

	enum class MemoryLocation : uint32_t {
		Unknown,
		L0_System,
		L1_Video
	};

	enum class MemoryHeapType : uint32_t {
		Default,
		Upload,
		ReadBack
	};

	struct IMemory : IObject {
	
	private:

		uint32_t			mSize;
		MemoryHostPaging	mHostPaging;
		MemoryLocation		mLocation;

	public:

		IMemory(IDevice * pDevice, uint32_t size, MemoryHostPaging hostPaging, MemoryLocation location);
		IMemory(IDevice * pDevice, uint32_t size, MemoryHeapType heapType);
		virtual ~IMemory() override;

		uint32_t size() const;
		MemoryHostPaging hostPaging() const;
		MemoryLocation location() const;

	};

}



#endif // ERHI_MEMORY_HPP