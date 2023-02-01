#include "erhi/vulkan/context/message.hpp"
#include "erhi/vulkan/context/instance.hpp"
#include "erhi/vulkan/context/physical_device.hpp"
#include "erhi/vulkan/context/device.hpp"

#include "erhi/vulkan/resource/memory.hpp"

#include <format>		// for log formatting
#include <utility>		// for std::pair used in enum mapping
#include <bit>			// for bit manipulation in selecting memory type
#include <cassert>		// for assert



namespace erhi::vk {

	Memory::Memory(DeviceHandle deviceHandle, MemoryDesc const & desc) :
		IMemory(desc),
		mDeviceHandle(std::move(deviceHandle)),
		mMemory(VK_NULL_HANDLE) {
		
		VkMemoryAllocateInfo const allocateInfo{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.pNext = nullptr,
			.allocationSize = desc.size,
			.memoryTypeIndex = desc.memoryTypeIndex
		};

		vkCheckResult(vkAllocateMemory(*mDeviceHandle, &allocateInfo, nullptr, &mMemory));
	}

	Memory::~Memory() {
		vkFreeMemory(*mDeviceHandle, mMemory, nullptr);
	}

	IDeviceHandle Memory::GetDevice() const {
		return mDeviceHandle;
	}



	static VkMemoryPropertyFlags MapHeapType(MemoryHeapType heapType) {
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

	static uint32_t GetMemoryTypeBits(PhysicalDevice * pPhysicalDevice, MemoryHeapType heapType) {
		auto const propertyFlags = MapHeapType(heapType);

		auto const & memoryProperties = pPhysicalDevice->mMemoryProperties.memoryProperties;

		auto memoryTypeBits = 0u;

		for (auto i = 0u; i < memoryProperties.memoryTypeCount; ++i) {
			if (memoryProperties.memoryTypes[i].propertyFlags == propertyFlags) {
				memoryTypeBits |= 1 << i;
			}
		}

		return memoryTypeBits;
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

	static VkBufferCreateInfo GetBufferCreateInfo(BufferDesc const & desc) {
		return VkBufferCreateInfo{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.size = desc.size,
			.usage = MapBufferUsage(desc.bufferUsage),
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = /* ignored */ 0,
			.pQueueFamilyIndices = /* ignored */ nullptr
		};
	}

	static uint32_t GetMemoryTypeIndex(PhysicalDevice * pPhysicalDevice, MemoryHeapType heapType, BufferDesc const & bufferDesc, uint32_t memoryTypeBits) {
		if (memoryTypeBits == 0) {
			throw invalid_arguments(std::format(
				"No available memory type for buffer with heap type = {}, usage = {}.\n",
				uint32_t(heapType), bufferDesc.bufferUsage
			));
		}

		if (std::popcount(memoryTypeBits) > 1) {
			pPhysicalDevice->mInstanceHandle->mMessageCallbackHandle->warning(std::format(
				"Multiple available memory types for buffer with heap type = {}, buffer usage = {}.\n",
				uint32_t(heapType), bufferDesc.bufferUsage
			));
		}

		return std::countr_zero(memoryTypeBits);
	}



	MemoryRequirements Device::GetBufferMemoryRequirements(MemoryHeapType heapType, BufferDesc const & bufferDesc) {
		
		// Select Vulkan memory types compatible with the input ERHI heap type, i.e., Vulkan memory property flags.

		uint32_t const propertyMemoryTypeBits = GetMemoryTypeBits(mPhysicalDeviceHandle.get(), heapType);

		// Select Vulkan memory types compatible with the buffer's meta data, mostly usage bits.

		VkBufferCreateInfo const createInfo = GetBufferCreateInfo(bufferDesc);

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

		return MemoryRequirements{
			.memoryTypeBits = memoryTypeBits,
			.size = memoryRequirement.memoryRequirements.size,
			.alignment = memoryRequirement.memoryRequirements.alignment
		};
	}



	IMemoryHandle Device::AllocateMemory(MemoryDesc const & desc) {
		return MakeHandle<Memory>(this, desc);
	}



	CommittedBuffer::CommittedBuffer(Device * pDevice, MemoryHeapType heapType, BufferDesc const & desc) :
		ICommittedBuffer(desc), mDeviceHandle(pDevice), mDeviceMemory(VK_NULL_HANDLE), mBuffer(VK_NULL_HANDLE) {
		
		// Get memory types compatible with the specified heap type.

		uint32_t const heapMemoryTypeBits = GetMemoryTypeBits(pDevice->mPhysicalDeviceHandle.get(), heapType);

		// Create a Vulkan buffer.

		VkBufferCreateInfo const bufferCreateInfo = GetBufferCreateInfo(desc);

		vkCheckResult(vkCreateBuffer(*mDeviceHandle, &bufferCreateInfo, nullptr, &mBuffer));

		// Query memory requirements for this buffer.

		VkMemoryRequirements2 memoryRequirements{
			.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2,
			.pNext = nullptr,
			.memoryRequirements = VkMemoryRequirements{
				.size = 0,
				.alignment = 1,
				.memoryTypeBits = 0
			}
		};

		VkMemoryDedicatedRequirements dedicatedRequirements{
			.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS,
			.pNext = nullptr,
			.prefersDedicatedAllocation = false,
			.requiresDedicatedAllocation = false
		};

		LinkNext(memoryRequirements, dedicatedRequirements);

		VkBufferMemoryRequirementsInfo2 const bufferMemoryRequirementsInfo{
			.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2,
			.pNext = nullptr,
			.buffer = mBuffer
		};

		vkGetBufferMemoryRequirements2(*mDeviceHandle, &bufferMemoryRequirementsInfo, &memoryRequirements);

		if (dedicatedRequirements.prefersDedicatedAllocation == false) {
			mDeviceHandle->mPhysicalDeviceHandle->mInstanceHandle->mMessageCallbackHandle->info(std::format(
				"Dedicated allocation is not preferred when memory heap type = {}, buffer usage = {}, size = {}.\n",
				uint32_t(heapType), desc.bufferUsage, desc.size
			));
		}

		// Get the final memory type, which is an intersection between those specified by heap type and buffer description.

		uint32_t const memoryTypeBits = heapMemoryTypeBits & memoryRequirements.memoryRequirements.memoryTypeBits;

		uint32_t const memoryTypeIndex = GetMemoryTypeIndex(mDeviceHandle->mPhysicalDeviceHandle.get(), heapType, desc, memoryTypeBits);

		// Allocate a dedicated memory for the buffer.

		VkMemoryAllocateInfo allocateInfo{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.pNext = nullptr,
			.allocationSize = memoryRequirements.memoryRequirements.size,
			.memoryTypeIndex = memoryTypeIndex
		};

		VkMemoryDedicatedAllocateInfo dedicatedAllocateInfo{
			.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO,
			.pNext = nullptr,
			.image = VK_NULL_HANDLE,
			.buffer = mBuffer
		};

		LinkNext(allocateInfo, dedicatedAllocateInfo);

		vkCheckResult(vkAllocateMemory(*mDeviceHandle, &allocateInfo, nullptr, &mDeviceMemory));

		// Bind buffer and memory.

		vkCheckResult(vkBindBufferMemory(*mDeviceHandle, mBuffer, mDeviceMemory, 0));

	}

	CommittedBuffer::~CommittedBuffer() {
		vkDestroyBuffer(*mDeviceHandle, mBuffer, nullptr);
		vkFreeMemory(*mDeviceHandle, mDeviceMemory, nullptr);
	}



	PlacedBuffer::PlacedBuffer(BufferDesc const & desc, MemoryHandle memoryHandle, uint64_t offset, uint64_t alignment) :
		IPlacedBuffer(desc, offset, alignment),
		mMemoryHandle(std::move(memoryHandle)),
		mBuffer(VK_NULL_HANDLE) {

		assert(offset % alignment == 0);

		VkBufferCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.size = desc.size,
			.usage = MapBufferUsage(desc.bufferUsage),
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr
		};

		vkCheckResult(vkCreateBuffer(mMemoryHandle->mDeviceHandle->mDevice, &createInfo, nullptr, &mBuffer));

		vkCheckResult(vkBindBufferMemory(mMemoryHandle->mDeviceHandle->mDevice, mBuffer, mMemoryHandle->mMemory, offset));
	}

	PlacedBuffer::~PlacedBuffer() {
		vkDestroyBuffer(mMemoryHandle->mDeviceHandle->mDevice, mBuffer, nullptr);
	}

	IMemoryHandle PlacedBuffer::GetMemory() const {
		return mMemoryHandle;
	}



	IPlacedBufferHandle Device::CreatePlacedBuffer(IMemoryHandle memoryHandle, uint64_t offset, uint64_t alignment, BufferDesc const & bufferDesc) {
		MemoryHandle handle = dynamic_handle_cast<Memory>(std::move(memoryHandle));
		return MakeHandle<PlacedBuffer>(bufferDesc, handle, offset, alignment);
	}

	ICommittedBufferHandle Device::CreateCommittedBuffer(MemoryHeapType heapType, BufferDesc const & bufferDesc) {
		return MakeHandle<CommittedBuffer>(this, heapType, bufferDesc);
	}

}