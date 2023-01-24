#pragma once

#include "../../common/resource/memory.hpp"
#include "../native.hpp"



namespace erhi::vk {

	struct Memory : IMemory {

		DeviceHandle mDeviceHandle;
		VkDeviceMemory mMemory;

		Memory(DeviceHandle deviceHandle, uint32_t memoryTypeIndex, VkDeviceSize size);
		virtual ~Memory() override;

		virtual IDeviceHandle		GetDevice() const override;

	};

	struct Buffer : IBuffer {

		MemoryHandle mMemoryHandle;
		VkBuffer mBuffer;

		Buffer(MemoryHandle memoryHandle, uint64_t offset, BufferDesc const & desc);
		virtual ~Buffer() override;

		virtual IMemoryHandle GetMemory() const override;

	};

}