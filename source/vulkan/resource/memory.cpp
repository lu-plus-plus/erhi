#include "erhi/vulkan/context/message.hpp"
#include "erhi/vulkan/context/instance.hpp"
#include "erhi/vulkan/context/physical_device.hpp"
#include "erhi/vulkan/context/device.hpp"

#include "erhi/vulkan/resource/memory.hpp"

#include <format>		// for log formatting
#include <utility>		// for std::pair used in enum mapping
#include <bit>			// for bit manipulation in selecting memory type



namespace erhi::vk {

	Memory::Memory(DeviceHandle deviceHandle, uint32_t memoryTypeIndex, VkDeviceSize size) :
		mDeviceHandle(std::move(deviceHandle)),
		mMemory(VK_NULL_HANDLE),
		mSize(size) {
		
		VkMemoryAllocateInfo const allocateInfo{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.pNext = nullptr,
			.allocationSize = size,
			.memoryTypeIndex = memoryTypeIndex
		};

		vkCheckResult(vkAllocateMemory(*mDeviceHandle, &allocateInfo, nullptr, &mMemory));
	}

	Memory::~Memory() {
		vkFreeMemory(*mDeviceHandle, mMemory, nullptr);
	}

	IDeviceHandle Memory::GetDevice() const {
		return mDeviceHandle;
	}

	uint64_t Memory::Size() const {
		return mSize;
	}



	static uint32_t MapHeapType(MemoryHeapType heapType) {
		VkMemoryPropertyFlags property = 0;

		switch (heapType) {
			case erhi::MemoryHeapType::Default: {
				property = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			} break;

			case erhi::MemoryHeapType::Upload: {
				property = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
			} break;

			case erhi::MemoryHeapType::ReadBack: {
				property = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
			} break;

			default: break;
		}

		return property;
	}

	static VkBufferUsageFlags MapBufferUsage(BufferUsageFlags inFlags) {
		static const std::pair<BufferUsageFlags, VkBufferUsageFlags> mappings[] = {
			{ BufferUsageBits::CopySource, VK_BUFFER_USAGE_TRANSFER_SRC_BIT },
			{ BufferUsageBits::CopyTarget, VK_BUFFER_USAGE_TRANSFER_DST_BIT },
			{ BufferUsageBits::UniformBuffer, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT },
			{ BufferUsageBits::StorageBuffer, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT },
			{ BufferUsageBits::IndexBuffer, VK_BUFFER_USAGE_INDEX_BUFFER_BIT },
			{ BufferUsageBits::VertexBuffer, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT },
		};

		VkBufferUsageFlags outFlags = 0;

		for (auto const & mapping : mappings) {
			if (inFlags & mapping.first) {
				outFlags |= mapping.second;
			}
		}

		return outFlags;
	}



	MemoryRequirements Device::GetBufferMemoryRequirements(MemoryHeapType heapType, BufferDesc const & bufferDesc) {
		
		// Select Vulkan memory types compatible with the input ERHI heap type, i.e., Vulkan memory property flags.

		auto const propertyFlags = MapHeapType(heapType);

		auto const & memoryProperties = mPhysicalDeviceHandle->mMemoryProperties.memoryProperties;

		auto propertyMemoryTypeBits = 0u;
		for (auto i = 0u; i < memoryProperties.memoryTypeCount; ++i) {
			if (memoryProperties.memoryTypes[i].propertyFlags == propertyFlags) {
				propertyMemoryTypeBits |= 1 << i;
			}
		}

		// Select Vulkan memory types compatible with the buffer's meta data, mostly usage bits.

		VkBufferCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.size = bufferDesc.size,
			.usage = MapBufferUsage(bufferDesc.bufferUsage),
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = /* ignored */ 0,
			.pQueueFamilyIndices = /* ignored */ nullptr
		};

		VkDeviceBufferMemoryRequirements const bufferMemoryRequirement{
			.sType = VK_STRUCTURE_TYPE_DEVICE_BUFFER_MEMORY_REQUIREMENTS,
			.pNext = nullptr,
			.pCreateInfo = &createInfo
		};

		VkMemoryRequirements2 memoryRequirement{
			.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2,
			.pNext = nullptr,
			.memoryRequirements = VkMemoryRequirements{
				.size = 0,
				.alignment = 1,
				.memoryTypeBits = 0
			}
		};

		vkGetDeviceBufferMemoryRequirements(mDevice, &bufferMemoryRequirement, &memoryRequirement);

		// Intersect two groups of memory types.

		uint32_t const memoryTypeBits = propertyMemoryTypeBits & memoryRequirement.memoryRequirements.memoryTypeBits;

		if (std::popcount(memoryTypeBits) != 1) {
			mPhysicalDeviceHandle->mInstanceHandle->mMessageCallbackHandle->warning(std::format(
				"Buffer has multiple memory types available: heap type = {}, buffer usage = {}.",
				uint32_t(heapType), uint32_t(bufferDesc.bufferUsage)
			));			
		}

		uint32_t const memoryTypeIndex = std::countr_zero(memoryTypeBits);

		return MemoryRequirements{
			.memoryTypeIndex = memoryTypeIndex,
			.size = memoryRequirement.memoryRequirements.size,
			.alignment = memoryRequirement.memoryRequirements.alignment
		};
	}



	IMemoryHandle Device::AllocateMemory(MemoryDesc const & desc) {
		return MakeHandle<Memory>(this, desc.memoryTypeIndex, desc.size);
	}

}