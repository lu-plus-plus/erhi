#pragma once

#include "../../common/resource/allocator.hpp"
#include "../native.hpp"

#include <vector>
#include <set>



namespace erhi::vk {

	template <typename T>
	struct AllocatorTraits {
		T allocator;

		VkResult Allocate(VkDeviceSize size, uint32_t memoryTypeIndex, VkDeviceMemory * pMemory) {
			return allocator.Allocate(size, memoryTypeIndex, pMemory);
		}

		void Release(VkDeviceMemory)
	};

	struct Allocator : IAllocator {

		DeviceHandle mDeviceHandle;

		std::vector<std::multiset<Chunk>> mHeapChunks;

		Allocator(Device * pDevice);
		virtual ~Allocator();

		virtual IDevice * pDevice() const;
		virtual std::optional<IMemory *> allocateMemory(uint32_t size, MemoryHeapType heapType, BufferUsageFlags bufferUsage);

	};
	
}