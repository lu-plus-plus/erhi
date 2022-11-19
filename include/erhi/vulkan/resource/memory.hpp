#ifndef ERHI_VULKAN_MEMORY_HPP
#define ERHI_VULKAN_MEMORY_HPP

#include <optional>		// for Memory::create()

#include "../../common/resource/memory.hpp"
#include "../native.hpp"



namespace erhi::vk {

	struct Memory : IMemory {
		DeviceHandle		mDeviceHandle;
		VkDeviceMemory		mMemory;

		Memory(Device * pDevice, uint32_t size, MemoryLocation location, MemoryHostAccess hostAccess);
		virtual ~Memory() override;

		virtual IDevice * pDevice() const override;
	};

}



#endif // ERHI_VULKAN_MEMORY_HPP