#ifndef ERHI_VULKAN_MEMORY_HPP
#define ERHI_VULKAN_MEMORY_HPP

#include "../../common/resource/memory.hpp"
#include "../native.hpp"



namespace erhi::vk {

	struct Memory : IMemory {
		VkDeviceMemory	mMemory;
		bool			mIsDeviceLocal;

		Memory(DeviceHandle pDevice, uint32_t size, MemoryHostAccess hostAccess, MemoryPool pool);
		virtual ~Memory() override;
	};

}



#endif // ERHI_VULKAN_MEMORY_HPP