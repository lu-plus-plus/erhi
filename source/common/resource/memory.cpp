#include "erhi/common/resource/memory.hpp"

namespace erhi {
	
	//IMemory::IMemory(IDevice * pDevice, uint32_t size, MemoryLocation location, MemoryHostAccess hostAccess) :
	//	IObject(), mSize(size), mLocation(location), mHostAccess(hostAccess) {}

	//IMemory::IMemory(IDevice * pDevice, uint32_t size, MemoryHeapType heapType) :
	//	IObject(), mSize(size), mLocation(), mHostAccess() {

	//	bool const isCacheCoherentUMA = pDevice->pPhysicalDevice()->isCacheCoherentUMA();

	//	if (isCacheCoherentUMA) {
	//		switch (heapType) {
	//			case MemoryHeapType::Default: {
	//				mLocation = MemoryLocation::L0_System;
	//				mHostAccess = MemoryHostAccess::SequentialWrite;
	//			}
	//			break;

	//			case MemoryHeapType::Upload: {
	//				pDevice->pPhysicalDevice()->pInstance()->mMessageCallbackHandle->warning("Requesting a memory for uploading data on cache-caherent UMA architecture.");
	//				mLocation = MemoryLocation::L0_System;
	//				mHostAccess = MemoryHostAccess::SequentialWrite;
	//			}
	//			break;

	//			case MemoryHeapType::ReadBack: {
	//				pDevice->pPhysicalDevice()->pInstance()->mMessageCallbackHandle->warning("Requesting a memory for reading data back on cache-caherent UMA architecture.");
	//				mLocation = MemoryLocation::L0_System;
	//				mHostAccess = MemoryHostAccess::Random;
	//			}

	//			default: break;
	//		}
	//	}
	//	else {
	//		switch (heapType) {
	//			case MemoryHeapType::Default: {
	//				mLocation = MemoryLocation::L1_Video;
	//				mHostAccess = MemoryHostAccess::NotAvailable;
	//			}
	//			break;

	//			case MemoryHeapType::Upload: {
	//				mLocation = MemoryLocation::L0_System;
	//				mHostAccess = MemoryHostAccess::SequentialWrite;
	//			}
	//			break;

	//			case MemoryHeapType::ReadBack: {
	//				mLocation = MemoryLocation::L0_System;
	//				mHostAccess = MemoryHostAccess::Random;
	//			}
	//			break;

	//			default: break;
	//		}
	//	}
	//}

	IMemory::IMemory(uint64_t size) : mSize(size) {}

	IMemory::~IMemory() = default;
	
	uint64_t IMemory::GetSize() const { return mSize; }



	IBuffer::IBuffer(uint64_t offsetInMemory, uint64_t size) : mOffsetInMemory(offsetInMemory), mSize(size) {}
	
	IBuffer::~IBuffer() = default;

	uint64_t IBuffer::OffsetInMemory() const { return mOffsetInMemory; }

	uint64_t IBuffer::Size() const { return mSize; }

}