#pragma once

#include "../../common/resource/memory.hpp"
#include "../native.hpp"



namespace erhi::vk {

	struct Memory : IMemory {

		DeviceHandle		mDeviceHandle;
		VkDeviceMemory		mMemory;
		VkDeviceSize		mSize;

		Memory(Device * pDevice, uint32_t memoryTypeIndex, VkDeviceSize size);
		virtual ~Memory() override;

		virtual IDevice * pDevice() const override;

		struct Fragment {
			MemoryHandle mMemoryHandle;
			VkDeviceSize mOffset;
			VkDeviceSize mSize;

			Fragment(Memory * pMemory, VkDeviceSize relativeOffset, VkDeviceSize size);
			
			Fragment Slice(VkDeviceSize relativeOffset, VkDeviceSize size);

			friend bool operator<(Fragment const & a, Fragment const & b);
		};

	};

}