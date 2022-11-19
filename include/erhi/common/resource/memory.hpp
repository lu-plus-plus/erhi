#ifndef ERHI_MEMORY_HPP
#define ERHI_MEMORY_HPP

#include "../object.hpp"
#include "../handle.hpp"

#include "../device.hpp"



namespace erhi {

	enum class MemoryLocation : uint32_t {
		L0_System,
		L1_Video,
	};

	enum class MemoryHostAccess : uint32_t {
		NotAvailable,
		SequentialWrite,
		Random
	};

	enum class MemoryHeapType : uint32_t {
		Default,
		Upload,
		ReadBack
	};

	struct IMemory : IObject, IOnDevice {
	
	private:

		uint32_t			mSize;
		MemoryLocation		mLocation;
		MemoryHostAccess	mHostAccess;

	public:

		IMemory(IDevice * pDevice, uint32_t size, MemoryLocation location, MemoryHostAccess hostAccess);
		IMemory(IDevice * pDevice, uint32_t size, MemoryHeapType heapType);
		virtual ~IMemory() override;

		virtual IDevice * pDevice() const override = 0;

		uint32_t size() const;
		MemoryHostAccess hostAccess() const;
		MemoryLocation location() const;

	};

}



#endif // ERHI_MEMORY_HPP