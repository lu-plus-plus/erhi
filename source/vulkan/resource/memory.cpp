#include "erhi/vulkan/context/message.hpp"
#include "erhi/vulkan/context/instance.hpp"
#include "erhi/vulkan/context/physical_device.hpp"
#include "erhi/vulkan/context/device.hpp"

#include "erhi/vulkan/resource/memory.hpp"

#include <format>		// for log formatting
#include <utility>		// for std::pair used in enum mapping
#include <bit>			// for bit manipulation in selecting memory type



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

	IMemoryHandle Device::AllocateMemory(MemoryDesc const & desc) {
		return MakeHandle<Memory>(this, desc);
	}



	static uint32_t MapHeapTypeToMemoryTypeBits(Device * pDevice, MemoryHeapType heapType) {

		auto GetVkMemoryPropertyFlags = [] (MemoryHeapType heapType) {
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
		};

		auto const propertyFlags = GetVkMemoryPropertyFlags(heapType);

		auto const & memoryProperties = pDevice->mPhysicalDeviceHandle->mMemoryProperties.memoryProperties;

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
			{ BufferUsageCopySource, VK_BUFFER_USAGE_TRANSFER_SRC_BIT },
			{ BufferUsageCopyTarget, VK_BUFFER_USAGE_TRANSFER_DST_BIT },
			{ BufferUsageUniformBuffer, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT },
			{ BufferUsageStorageBuffer, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT },
			{ BufferUsageIndexBuffer, VK_BUFFER_USAGE_INDEX_BUFFER_BIT },
			{ BufferUsageVertexBuffer, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT },
		};

		VkBufferUsageFlags outFlags = 0;

		for (auto const & mapping : mappings) {
			if ((inFlags & mapping.first) == mapping.first) {
				outFlags |= mapping.second;
			}
		}

		return outFlags;
	}

	static VkBufferCreateInfo MapBufferDescToVkBufferCreateInfo(BufferDesc const & desc) {
		return VkBufferCreateInfo{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.size = desc.size,
			.usage = MapBufferUsage(desc.usage),
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = /* ignored */ 0,
			.pQueueFamilyIndices = /* ignored */ nullptr
		};
	}



	MemoryRequirements Device::GetBufferMemoryRequirements(MemoryHeapType heapType, BufferDesc const & bufferDesc) {
	
		// Select Vulkan memory types compatible with the input ERHI heap type, i.e., Vulkan memory property flags.

		uint32_t const heapMemoryTypeBits = MapHeapTypeToMemoryTypeBits(this, heapType);

		// Select Vulkan memory types compatible with the buffer's meta data, mostly usage bits.

		VkMemoryRequirements2 memoryRequirements{
			.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2,
			.pNext = nullptr
		};

		VkMemoryDedicatedRequirements dedicatedRequirements{
			.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS,
			.pNext = nullptr
		};
		
		NextChain(memoryRequirements).Next(dedicatedRequirements);

		{
			VkBufferCreateInfo const createInfo = MapBufferDescToVkBufferCreateInfo(bufferDesc);
			
			VkDeviceBufferMemoryRequirements const bufferMemoryRequirements{
				.sType = VK_STRUCTURE_TYPE_DEVICE_BUFFER_MEMORY_REQUIREMENTS,
				.pNext = nullptr,
				.pCreateInfo = &createInfo
			};

			vkGetDeviceBufferMemoryRequirements(mDevice, &bufferMemoryRequirements, &memoryRequirements);
		}

		// Intersect two groups of memory types.

		uint32_t const memoryTypeBits = heapMemoryTypeBits & memoryRequirements.memoryRequirements.memoryTypeBits;

		// <todo> Report warnings when there are multiple types available. </todo>

		return MemoryRequirements{
			.memoryTypeBits = memoryTypeBits,
			.prefersCommittedResource = dedicatedRequirements.prefersDedicatedAllocation != 0u,
			.requiresCommittedResource = dedicatedRequirements.requiresDedicatedAllocation != 0u,
			.size = memoryRequirements.memoryRequirements.size,
			.alignment = memoryRequirements.memoryRequirements.alignment
		};
	}



	CommittedBuffer::CommittedBuffer(Device * pDevice, MemoryHeapType heapType, BufferDesc const & desc) :
		IBuffer(desc), mDeviceHandle(pDevice), mDeviceMemory(VK_NULL_HANDLE), mBuffer(VK_NULL_HANDLE) {
		
		// Get memory types compatible with the specified heap type.

		uint32_t const heapMemoryTypeBits = MapHeapTypeToMemoryTypeBits(pDevice, heapType);

		// Create a Vulkan buffer.

		VkBufferCreateInfo const bufferCreateInfo = MapBufferDescToVkBufferCreateInfo(desc);

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

		NextChain(memoryRequirements).Next(dedicatedRequirements);

		VkBufferMemoryRequirementsInfo2 const bufferMemoryRequirementsInfo{
			.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2,
			.pNext = nullptr,
			.buffer = mBuffer
		};

		vkGetBufferMemoryRequirements2(*mDeviceHandle, &bufferMemoryRequirementsInfo, &memoryRequirements);

		if (dedicatedRequirements.prefersDedicatedAllocation == false) {
			mDeviceHandle->mPhysicalDeviceHandle->mInstanceHandle->mMessageCallbackHandle->Info(std::format(
				"Dedicated allocation is not preferred when memory heap type = {}, buffer usage = {}.\n",
				uint32_t(heapType), desc.usage
			));
		}

		// Get the final memory type, which is an intersection between those specified by heap type and buffer description.

		uint32_t const memoryTypeBits = heapMemoryTypeBits & memoryRequirements.memoryRequirements.memoryTypeBits;

		if (not std::has_single_bit(memoryTypeBits)) {
			mDeviceHandle->mPhysicalDeviceHandle->mInstanceHandle->mMessageCallbackHandle->Warning(std::format(
				"Multiple memory types are available when memory heap type = {}, buffer usage = {}.\n",
				uint32_t(heapType), desc.usage
			));
		}

		uint32_t const memoryTypeIndex = std::countr_zero(memoryTypeBits);

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

		NextChain(allocateInfo).Next(dedicatedAllocateInfo);

		vkCheckResult(vkAllocateMemory(*mDeviceHandle, &allocateInfo, nullptr, &mDeviceMemory));

		// Bind buffer and memory.

		vkCheckResult(vkBindBufferMemory(*mDeviceHandle, mBuffer, mDeviceMemory, 0));

	}

	CommittedBuffer::~CommittedBuffer() {
		vkDestroyBuffer(*mDeviceHandle, mBuffer, nullptr);
		vkFreeMemory(*mDeviceHandle, mDeviceMemory, nullptr);
	}



	VkBuffer Memory::CreateNativeBuffer(uint64_t offset, uint64_t actualSize, BufferDesc const & desc) {
		VkBuffer buffer = VK_NULL_HANDLE;

		VkBufferCreateInfo createInfo = MapBufferDescToVkBufferCreateInfo(desc);

		vkCheckResult(vkCreateBuffer(mDeviceHandle->mDevice, &createInfo, nullptr, &buffer));

		vkCheckResult(vkBindBufferMemory(mDeviceHandle->mDevice, buffer, mMemory, offset));

		return buffer;
	}

	void Memory::DestroyNativeBuffer(VkBuffer buffer) {
		vkDestroyBuffer(mDeviceHandle->mDevice, buffer, nullptr);
	}



	IBufferHandle Memory::CreatePlacedBuffer(uint64_t offset, uint64_t actualSize, BufferDesc const & bufferDesc) {
		return MakeHandle<PlacedBuffer<Slice>>(
			Slice{ .mMemoryHandle = this, .mOffset = offset, .mSize = actualSize },
			bufferDesc
		);
	}

	IBufferHandle Device::CreateCommittedBuffer(MemoryHeapType heapType, BufferDesc const & bufferDesc) {
		return MakeHandle<CommittedBuffer>(this, heapType, bufferDesc);
	}



	VkFormat MapFormat(Format format) {
		switch (format) {
			case erhi::Format::Unknown:
				return VK_FORMAT_UNDEFINED;
				break;

			case erhi::Format::R32G32B32A32_Typeless:
			case erhi::Format::R32G32B32A32_Float:
				return VK_FORMAT_R32G32B32A32_SFLOAT;
				break;
			case erhi::Format::R32G32B32A32_UInt:
				return VK_FORMAT_R32G32B32A32_UINT;
				break;
			case erhi::Format::R32G32B32A32_SInt:
				return VK_FORMAT_R32G32B32A32_SINT;
				break;
			
			case erhi::Format::R32G32B32_Typeless:
			case erhi::Format::R32G32B32_Float:
				return VK_FORMAT_R32G32B32_SFLOAT;
				break;
			case erhi::Format::R32G32B32_UInt:
				return VK_FORMAT_R32G32B32_UINT;
				break;
			case erhi::Format::R32G32B32_SInt:
				return VK_FORMAT_R32G32B32_SINT;
				break;

			case erhi::Format::R16G16B16A16_Typeless:
			case erhi::Format::R16G16B16A16_Float:
				return VK_FORMAT_R16G16B16A16_SFLOAT;
				break;
			case erhi::Format::R16G16B16A16_UNorm:
				return VK_FORMAT_R16G16B16A16_UNORM;
				break;
			case erhi::Format::R16G16B16A16_UInt:
				return VK_FORMAT_R16G16B16A16_UINT;
				break;
			case erhi::Format::R16G16B16A16_SNorm:
				return VK_FORMAT_R16G16B16A16_SNORM;
				break;
			case erhi::Format::R16G16B16A16_SInt:
				return VK_FORMAT_R16G16B16A16_SINT;
				break;

			case erhi::Format::R32G32_Typeless:
			case erhi::Format::R32G32_Float:
				return VK_FORMAT_R32G32_SFLOAT;
				break;
			case erhi::Format::R32G32_UInt:
				return VK_FORMAT_R32G32_UINT;
				break;
			case erhi::Format::R32G32_SInt:
				return VK_FORMAT_R32G32_SINT;
				break;
			
			case erhi::Format::R8G8B8A8_Typeless:
			case erhi::Format::R8G8B8A8_UNorm:
				return VK_FORMAT_R8G8B8A8_UNORM;
				break;
			case erhi::Format::R8G8B8A8_UInt:
				return VK_FORMAT_R8G8B8A8_UINT;
				break;
			case erhi::Format::R8G8B8A8_SNorm:
				return VK_FORMAT_R8G8B8A8_SNORM;
				break;
			case erhi::Format::R8G8B8A8_SInt:
				return VK_FORMAT_R8G8B8A8_SINT;
				break;
			
			case erhi::Format::R16G16_Typeless:
			case erhi::Format::R16G16_Float:
				return VK_FORMAT_R16G16_SFLOAT;
				break;
			case erhi::Format::R16G16_UNorm:
				return VK_FORMAT_R16G16_UNORM;
				break;
			case erhi::Format::R16G16_UInt:
				return VK_FORMAT_R16G16_UINT;
				break;
			case erhi::Format::R16G16_SNorm:
				return VK_FORMAT_R16G16_SNORM;
				break;
			case erhi::Format::R16G16_SInt:
				return VK_FORMAT_R16G16_SINT;
				break;
			
			case erhi::Format::R32_Float:
				return VK_FORMAT_R32_SFLOAT;
				break;
			case erhi::Format::R32_UInt:
				return VK_FORMAT_R32_UINT;
				break;
			case erhi::Format::R32_SInt:
				return VK_FORMAT_R32_SINT;
				break;
			
			case erhi::Format::D32_Float:
				return VK_FORMAT_D32_SFLOAT;
				break;
			case erhi::Format::D16_UNorm:
				return VK_FORMAT_D16_UNORM;
				break;
			case erhi::Format::D24_UNorm_S8_UInt:
				return VK_FORMAT_D24_UNORM_S8_UINT;
				break;
			
			default:
				return VK_FORMAT_UNDEFINED;
				break;
		}
	}

	static VkSampleCountFlagBits MapSampleCount(TextureSampleCount sampleCount) {
		return static_cast<VkSampleCountFlagBits>(1u << static_cast<uint32_t>(sampleCount));
	}

	static VkImageUsageFlags MapTextureUsage(TextureUsageFlags flags) {
		VkImageUsageFlags result = 0u;
		if (flags & TextureUsageCopySource) {
			result |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		}
		if (flags & TextureUsageCopyTarget) {
			result |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}
		if (flags & TextureUsageSampled) {
			result |= VK_IMAGE_USAGE_SAMPLED_BIT;
		}
		if (flags & TextureUsageStorage) {
			result |= VK_IMAGE_USAGE_STORAGE_BIT;
		}
		if (flags & TextureUsageRenderTargetAttachment) {
			result |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		}
		if (flags & TextureUsageDepthStencilAttachment) {
			result |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		}
		return result;
	}



	static VkImageCreateInfo GetImageCreateInfo(TextureDesc const & desc) {
		return VkImageCreateInfo{
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.imageType = static_cast<VkImageType>(desc.dimension),
			.format = MapFormat(desc.format),
			.extent = VkExtent3D{ .width = desc.extent[0], .height = desc.extent[1], .depth = desc.extent[2] },
			.mipLevels = desc.mipLevels,
			.arrayLayers = 1u,
			.samples = MapSampleCount(desc.sampleCount),
			.tiling = static_cast<VkImageTiling>(desc.tiling),
			.usage = MapTextureUsage(desc.usage),
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
		};
	}

	CommittedTexture::CommittedTexture(Device * pDevice, MemoryHeapType heapType, TextureDesc const & desc) :
		ITexture(desc), mDeviceHandle(pDevice), mDeviceMemory(VK_NULL_HANDLE), mImage(VK_NULL_HANDLE) {

		// Get memory types compatible with the specified heap type.

		uint32_t const heapMemoryTypeBits = MapHeapTypeToMemoryTypeBits(pDevice, heapType);

		// Create a Vulkan image.

		VkImageCreateInfo const imageCreateInfo = GetImageCreateInfo(desc);

		vkCheckResult(vkCreateImage(*mDeviceHandle, &imageCreateInfo, nullptr, &mImage));

		// Query memory requirements for this texture.

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

		NextChain(memoryRequirements).Next(dedicatedRequirements);

		VkImageMemoryRequirementsInfo2 const imageMemoryRequirementsInfo{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2,
			.pNext = nullptr,
			.image = mImage
		};

		vkGetImageMemoryRequirements2(*mDeviceHandle, &imageMemoryRequirementsInfo, &memoryRequirements);

		if (dedicatedRequirements.prefersDedicatedAllocation == false) {
			mDeviceHandle->mPhysicalDeviceHandle->mInstanceHandle->mMessageCallbackHandle->Info(std::format(
				"Dedicated allocation is not preferred when memory heap type = {}, texture usage = {}.\n",
				uint32_t(heapType), desc.usage
			));
		}

		// Get the final memory type, which is an intersection between those specified by heap type and buffer description.

		uint32_t const memoryTypeBits = heapMemoryTypeBits & memoryRequirements.memoryRequirements.memoryTypeBits;

		if (not std::has_single_bit(memoryTypeBits)) {
			mDeviceHandle->mPhysicalDeviceHandle->mInstanceHandle->mMessageCallbackHandle->Warning(std::format(
				"Multiple memory types are available when memory heap type = {}, buffer usage = {}.\n",
				uint32_t(heapType), desc.usage
			));
		}

		uint32_t const memoryTypeIndex = std::countr_zero(memoryTypeBits);

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
			.image = mImage,
			.buffer = VK_NULL_HANDLE
		};

		NextChain(allocateInfo).Next(dedicatedAllocateInfo);

		vkCheckResult(vkAllocateMemory(*mDeviceHandle, &allocateInfo, nullptr, &mDeviceMemory));

		// Bind buffer and memory.

		vkCheckResult(vkBindImageMemory(*mDeviceHandle, mImage, mDeviceMemory, 0));

	}

	CommittedTexture::~CommittedTexture() {
		vkDestroyImage(*mDeviceHandle, mImage, nullptr);
		vkFreeMemory(*mDeviceHandle, mDeviceMemory, nullptr);
	}

	ITextureHandle Device::CreateCommittedTexture(MemoryHeapType heapType, TextureDesc const & textureDesc) {
		return MakeHandle<CommittedTexture>(this, heapType, textureDesc);
	}



	MemoryRequirements Device::GetTextureMemoryRequirements(MemoryHeapType heapType, TextureDesc const & textureDesc) {

		// Select Vulkan memory types compatible with the input ERHI heap type, i.e., Vulkan memory property flags.

		uint32_t const heapMemoryTypeBits = MapHeapTypeToMemoryTypeBits(this, heapType);

		// Select Vulkan memory types compatible with the buffer's meta data, mostly usage bits.

		VkMemoryRequirements2 memoryRequirements{
			.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2,
			.pNext = nullptr
		};

		VkMemoryDedicatedRequirements dedicatedRequirements{
			.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS,
			.pNext = nullptr
		};

		NextChain(memoryRequirements).Next(dedicatedRequirements);

		{
			VkImageCreateInfo const createInfo = GetImageCreateInfo(textureDesc);

			VkDeviceImageMemoryRequirements const imageMemoryRequirements{
				.sType = VK_STRUCTURE_TYPE_DEVICE_BUFFER_MEMORY_REQUIREMENTS,
				.pNext = nullptr,
				.pCreateInfo = &createInfo
			};

			vkGetDeviceImageMemoryRequirements(mDevice, &imageMemoryRequirements, &memoryRequirements);
		}

		// Intersect two groups of memory types.

		uint32_t const memoryTypeBits = heapMemoryTypeBits & memoryRequirements.memoryRequirements.memoryTypeBits;

		// <todo> Report warnings when there are multiple types available. </todo>

		return MemoryRequirements{
			.memoryTypeBits = memoryTypeBits,
			.prefersCommittedResource = dedicatedRequirements.prefersDedicatedAllocation != 0u,
			.requiresCommittedResource = dedicatedRequirements.requiresDedicatedAllocation != 0u,
			.size = memoryRequirements.memoryRequirements.size,
			.alignment = memoryRequirements.memoryRequirements.alignment
		};
	}



	PlacedTexture::PlacedTexture(Memory * pMemory, uint64_t offset, uint64_t actualSize, TextureDesc const & textureDesc) :
		ITexture(textureDesc),
		mMemoryHandle(pMemory), mOffset(offset), mActualSize(actualSize) {

		VkImageCreateInfo createInfo = GetImageCreateInfo(textureDesc);

		vkCheckResult(vkCreateImage(mMemoryHandle->mDeviceHandle->mDevice, &createInfo, nullptr, &mImage));

		vkCheckResult(vkBindImageMemory(mMemoryHandle->mDeviceHandle->mDevice, mImage, mMemoryHandle->mMemory, offset));
	}

	PlacedTexture::~PlacedTexture() {
		vkDestroyImage(mMemoryHandle->mDeviceHandle->mDevice, mImage, nullptr);
	}

	ITextureHandle Memory::CreatePlacedTexture(uint64_t offset, uint64_t actualSize, TextureDesc const & textureDesc) {
		return MakeHandle<PlacedTexture>(this, offset, actualSize, textureDesc);
	}

}