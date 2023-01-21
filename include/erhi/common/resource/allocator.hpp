#pragma once

#include <optional>

#include "../common.hpp"



namespace erhi {

	struct IAllocator : IObject {

		IAllocator();
		virtual ~IAllocator();

		virtual IDevice * pDevice() const = 0;
		virtual std::optional<IMemory *> createBuffer(uint32_t size, MemoryHeapType heapType, BufferUsageFlags bufferUsage) = 0;

	};

	[[nodiscard]]
	IAllocatorHandle createAllocator(IDevice * pDevice);

}