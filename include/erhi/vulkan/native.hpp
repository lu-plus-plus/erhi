#pragma once

#include "volk.h"

#define VMA_VULKAN_VERSION 1003000
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#include "vk_mem_alloc.h"

#include "../common/handle.hpp"			// forward declaration for abstract interfaces
#include "../common/common.hpp"			// descriptive structs and enums
#include "../common/exception.hpp"		// exception



namespace erhi::vk {

	char const * vkErrorCode(VkResult result);

	template <typename T>
	concept HasConstNext = requires (T t) {
		{ t.pNext } -> std::same_as<void const * &>;
	};

	template <typename T>
	concept HasNext = requires (T t) {
		{ t.pNext } -> std::same_as<void * &>;
	};

	struct NextChain {
		void * * ppNext;

		template <HasConstNext T>
		NextChain(T & t) : ppNext(const_cast<void * *>(&t.pNext)) {}

		template <HasNext T>
		NextChain(T & t) : ppNext(&t.pNext) {}

		template <HasConstNext T>
		NextChain & Next(T & t) {
			t.pNext = *ppNext;
			*ppNext = &t;
			ppNext = const_cast<void * *>(&t.pNext);
			return *this;
		}

		template <HasNext T>
		NextChain & Next(T & t) {
			t.pNext = *ppNext;
			*ppNext = &t;
			ppNext = &t.pNext;
			return *this;
		}
	};

}

#define vkCheckResult(result) if (VkResult r = (result); r != VK_SUCCESS) throw bad_graphics_api_call(vkErrorCode(r), #result, std::source_location::current())



namespace erhi::vk::mapping {

	[[deprecated]]
	uint32_t MapHeapTypeToMemoryTypeBits(VkPhysicalDeviceMemoryProperties const & memoryProperties, MemoryHeapType heapType);
	
	VmaAllocationCreateInfo MapHeapType(MemoryHeapType heapType);

	VkBufferUsageFlags MapBufferUsage(BufferUsageFlags flags);
	VkBufferCreateInfo MapBufferCreateInfo(BufferDesc const & desc);

	VkFormat MapFormat(Format format);

	VkDescriptorType MapDescriptorType(DescriptorType type);

	VkShaderStageFlags MapShaderStageFlags(ShaderStageFlags flags);

}