#pragma once

#include "../../common/resource/memory.hpp"
#include "../native.hpp"



namespace erhi::vk {

	struct Memory : IMemory {

		DeviceHandle		mDeviceHandle;
		VkDeviceMemory		mMemory;

		Memory(DeviceHandle deviceHandle, MemoryDesc const & desc);
		virtual ~Memory() override;

		virtual IDeviceHandle GetDevice() const override;

	};



	struct CommittedBuffer : ICommittedBuffer {

		DeviceHandle		mDeviceHandle;
		VkDeviceMemory		mDeviceMemory;
		VkBuffer			mBuffer;

		CommittedBuffer(Device * pDevice, MemoryHeapType heapType, BufferDesc const & desc);
		virtual ~CommittedBuffer() override;

	};



	struct PlacedBuffer : IPlacedBuffer {

		MemoryHandle		mMemoryHandle;
		VkBuffer			mBuffer;

		PlacedBuffer(BufferDesc const & desc, MemoryHandle memoryHandle, uint64_t offset, uint64_t alignment);
		virtual ~PlacedBuffer() override;

		virtual IMemoryHandle GetMemory() const override;

	};

}