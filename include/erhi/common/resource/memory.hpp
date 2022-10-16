#ifndef ERHI_MEMORY_HPP
#define ERHI_MEMORY_HPP

#include "../object.hpp"
#include "../handle.hpp"



namespace erhi {

	enum class MemoryHostAccess : uint32_t {
		Auto,
		NotAvailable,
		SequentialWrite,
		Random
	};

	enum class MemoryLocation : uint32_t {
		L0_System,
		L1_Video,
		DeviceLocal
	};

	enum class MemoryHeapType : uint32_t {
		Default,
		Upload,
		ReadBack
	};

	struct IMemory : IObject {
	
	private:

		uint32_t			mSize;
		MemoryHostAccess	mHostAccess;
		MemoryLocation		mLocation;

	public:

		IMemory(IDevice * pDevice, uint32_t size, MemoryHostAccess hostAccess, MemoryLocation location);
		IMemory(IDevice * pDevice, uint32_t size, MemoryHeapType heapType);
		virtual ~IMemory() override;

		uint32_t size() const;
		MemoryHostAccess hostAccess() const;
		MemoryLocation location() const;

	};

}



#endif // ERHI_MEMORY_HPP