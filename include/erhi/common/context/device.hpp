#ifndef ERHI_DEVICE_HPP
#define ERHI_DEVICE_HPP

#include "../common.hpp"



namespace erhi {

	struct IDevice : IObject {

		IDevice();
		virtual ~IDevice();

		virtual IPhysicalDeviceHandle		GetPhysicalDevice() const = 0;

		virtual IQueueHandle				SelectQueue(QueueType queueType) = 0;

		virtual IMemoryHandle				AllocateMemory(MemoryDesc const & desc) = 0;
		virtual MemoryRequirements			GetBufferMemoryRequirements(MemoryHeapType heapType, BufferDesc const & bufferDesc) = 0;
		virtual IPlacedBufferHandle			CreatePlacedBuffer(IMemoryHandle memoryHandle, uint64_t offset, uint64_t alignment, BufferDesc const & bufferDesc) = 0;
		virtual ICommittedBufferHandle		CreateCommittedBuffer(MemoryHeapType heapType, BufferDesc const & bufferDesc) = 0;

	};

}



#endif // ERHI_DEVICE_HPP