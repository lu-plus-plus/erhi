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
	
	IMemory::IMemory(MemoryDesc const & desc) : mDesc(desc) {}

	IMemory::~IMemory() = default;
	
	MemoryDesc const & IMemory::GetDesc() const { return mDesc; }



	IBuffer::IBuffer(BufferDesc const & desc) : mDesc(desc) {}

	IBuffer::~IBuffer() = default;

	BufferDesc const & IBuffer::GetDesc() const { return mDesc; }



	//ICommittedBuffer::ICommittedBuffer(BufferDesc const & desc) : IBuffer(desc) {}

	//ICommittedBuffer::~ICommittedBuffer() = default;

	//bool ICommittedBuffer::IsCommittedResource() const { return true; }



	//IPlacedBuffer::IPlacedBuffer(BufferDesc const & desc, uint64_t offsetInMemory, uint64_t alignment) :
	//	IBuffer(desc), mOffsetInMemory(offsetInMemory), mAlignment(alignment) {}
	//
	//IPlacedBuffer::~IPlacedBuffer() = default;

	//bool IPlacedBuffer::IsCommittedResource() const { return false; }



	ITexture::ITexture(TextureDesc const & desc) : mDesc(desc) {}

	ITexture::~ITexture() = default;

	TextureDesc const & ITexture::GetDesc() const { return mDesc; }



	//ICommittedTexture::ICommittedTexture(TextureDesc const & desc) : ITexture(desc) {}

	//ICommittedTexture::~ICommittedTexture() = default;



	//IPlacedTexture::IPlacedTexture(TextureDesc const & desc, uint64_t offsetInMemory, uint64_t alignment) : ITexture(desc), mOffsetInMemory(offsetInMemory), mAlignment(alignment) {}

	//IPlacedTexture::~IPlacedTexture() = default;

}