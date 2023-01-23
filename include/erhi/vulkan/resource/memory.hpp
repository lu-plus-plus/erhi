#pragma once

#include "../../common/resource/memory.hpp"
#include "../native.hpp"



namespace erhi::vk {

	struct Memory : IMemory {

		DeviceHandle		mDeviceHandle;
		VkDeviceMemory		mMemory;
		VkDeviceSize		mSize;

		Memory(DeviceHandle deviceHandle, uint32_t memoryTypeIndex, VkDeviceSize size);
		virtual ~Memory() override;

		virtual IDeviceHandle	GetDevice() const override;
		virtual uint64_t		Size() const override;

	};

}