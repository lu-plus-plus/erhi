#include "erhi/vulkan/context/message.hpp"
#include "erhi/vulkan/context/instance.hpp"
#include "erhi/vulkan/context/physical_device.hpp"
#include "erhi/vulkan/context/device.hpp"

#include "erhi/vulkan/resource/memory.hpp"

#include <format>		// for log formatting
#include <utility>		// for std::pair used in enum mapping
#include <bit>			// for bit manipulation in selecting memory type

#include "magic_enum_format.hpp"



namespace erhi::vk {

	Memory::Memory(DeviceHandle deviceHandle, MemoryRequirements const & requirements) :
		IMemory(requirements),
		mDeviceHandle(std::move(deviceHandle)),
		mAllocation(VK_NULL_HANDLE) {
		
		VkMemoryRequirements vkRequirements = {
			.size = requirements.size,
			.alignment = requirements.alignment,
			.memoryTypeBits = requirements.memoryTypeBits
		};

		VmaAllocationCreateInfo createInfo = {};
		createInfo.flags = VMA_ALLOCATION_CREATE_CAN_ALIAS_BIT;

		vkCheckResult(vmaAllocateMemory(mDeviceHandle->mAllocator, &vkRequirements, &createInfo, &mAllocation, nullptr));
	}

	Memory::~Memory() {
		vmaFreeMemory(mDeviceHandle->mAllocator, mAllocation);
	}

	IDeviceHandle Memory::GetDevice() const {
		return mDeviceHandle;
	}

	IMemoryHandle Device::AllocateMemory(MemoryRequirements const & requirements) {
		return MakeHandle<Memory>(this, requirements);
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

	static VmaAllocationCreateInfo MapHeapType(MemoryHeapType heapType) {
		VmaAllocationCreateInfo createInfo = {};
		switch (heapType) {
			case erhi::MemoryHeapType::Default:
				createInfo.flags = 0u;
				createInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
				break;
			case erhi::MemoryHeapType::Upload:
				createInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
				createInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
				break;
			case erhi::MemoryHeapType::ReadBack:
				createInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
				createInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
				break;
			default:
				break;
		}
		return createInfo;
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

	static VkBufferCreateInfo GetBufferCreateInfo(BufferDesc const & desc) {
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
			VkBufferCreateInfo const createInfo = GetBufferCreateInfo(bufferDesc);
			
			VkDeviceBufferMemoryRequirements const bufferMemoryRequirements{
				.sType = VK_STRUCTURE_TYPE_DEVICE_BUFFER_MEMORY_REQUIREMENTS,
				.pNext = nullptr,
				.pCreateInfo = &createInfo
			};

			vkGetDeviceBufferMemoryRequirements(mDevice, &bufferMemoryRequirements, &memoryRequirements);
		}

		// Two groups of memory types are intersected.

		uint32_t const memoryTypeBits = heapMemoryTypeBits & memoryRequirements.memoryRequirements.memoryTypeBits;

		return MemoryRequirements{
			.memoryTypeBits = memoryTypeBits,
			.prefersCommittedResource = dedicatedRequirements.prefersDedicatedAllocation != 0u,
			.requiresCommittedResource = dedicatedRequirements.requiresDedicatedAllocation != 0u,
			.pageTypeIndex = 0,
			.size = memoryRequirements.memoryRequirements.size,
			.alignment = memoryRequirements.memoryRequirements.alignment
		};
	}



	Buffer::Buffer(Device * pDevice, MemoryHeapType heapType, BufferDesc const & bufferDesc) : IBuffer(bufferDesc), mDeviceHandle(pDevice), mAllocation(VK_NULL_HANDLE), mBuffer(VK_NULL_HANDLE) {
		VkBufferCreateInfo const bufferCreateInfo = GetBufferCreateInfo(bufferDesc);
		VmaAllocationCreateInfo allocationCreateInfo = MapHeapType(heapType);
		vkCheckResult(vmaCreateBuffer(pDevice->mAllocator, &bufferCreateInfo, &allocationCreateInfo, &mBuffer, &mAllocation, nullptr));
	}

	Buffer::~Buffer() {
		vmaDestroyBuffer(mDeviceHandle->mAllocator, mBuffer, mAllocation);
	}

	IBufferHandle Device::CreateBuffer(MemoryHeapType heapType, BufferDesc const & bufferDesc) {
		return MakeHandle<Buffer>(this, heapType, bufferDesc);
	}



	PlacedBuffer::PlacedBuffer(Memory * pMemory, uint64_t offset, BufferDesc const & desc) : IBuffer(desc), mMemoryHandle(pMemory), mBuffer(VK_NULL_HANDLE) {
		VkBufferCreateInfo const createInfo = GetBufferCreateInfo(desc);
		vkCheckResult(vkCreateBuffer(mMemoryHandle->mDeviceHandle->mDevice, &createInfo, nullptr, &mBuffer));

		vkCheckResult(vmaBindBufferMemory2(mMemoryHandle->mDeviceHandle->mAllocator, mMemoryHandle->mAllocation, offset, mBuffer, nullptr));
	}

	PlacedBuffer::~PlacedBuffer() {
		vkDestroyBuffer(mMemoryHandle->mDeviceHandle->mDevice, mBuffer, nullptr);
	}

	IBufferHandle Device::CreatePlacedBuffer(IMemoryHandle memory, uint64_t offset, BufferDesc const & desc) {
		auto pMemory = dynamic_cast<Memory *>(memory.get());
		return MakeHandle<PlacedBuffer>(pMemory, offset, desc);
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

		return MemoryRequirements{
			.memoryTypeBits = memoryTypeBits,
			.prefersCommittedResource = dedicatedRequirements.prefersDedicatedAllocation != 0u,
			.requiresCommittedResource = dedicatedRequirements.requiresDedicatedAllocation != 0u,
			.pageTypeIndex = uint8_t(textureDesc.tiling == TextureTiling::Optimal ? 1 : 0),
			.size = memoryRequirements.memoryRequirements.size,
			.alignment = memoryRequirements.memoryRequirements.alignment
		};
	}



	Texture::Texture(Device * pDevice, MemoryHeapType heapType, TextureDesc const & textureDesc) : ITexture(textureDesc), mDeviceHandle(pDevice), mAllocation(VK_NULL_HANDLE), mImage(VK_NULL_HANDLE) {
		VkImageCreateInfo const imageCreate = GetImageCreateInfo(textureDesc);

		VmaAllocationCreateInfo const allocationCreate = MapHeapType(heapType);

		vkCheckResult(vmaCreateImage(mDeviceHandle->mAllocator, &imageCreate, &allocationCreate, &mImage, &mAllocation, nullptr));
	}

	Texture::~Texture() {
		vmaDestroyImage(mDeviceHandle->mAllocator, mImage, mAllocation);
	}

	ITextureHandle Device::CreateTexture(MemoryHeapType heapType, TextureDesc const & textureDesc) {
		return MakeHandle<Texture>(this, heapType, textureDesc);
	}



	PlacedTexture::PlacedTexture(Memory * pMemory, uint64_t offset, TextureDesc const & textureDesc) : ITexture(textureDesc), mMemoryHandle(pMemory), mImage(VK_NULL_HANDLE) {
		auto const createInfo = GetImageCreateInfo(textureDesc);
		vkCheckResult(vkCreateImage(mMemoryHandle->mDeviceHandle->mDevice, &createInfo, nullptr, &mImage));
		vkCheckResult(vmaBindImageMemory2(mMemoryHandle->mDeviceHandle->mAllocator, mMemoryHandle->mAllocation, offset, mImage, nullptr));
	}

	PlacedTexture::~PlacedTexture() {
		vkDestroyImage(mMemoryHandle->mDeviceHandle->mDevice, mImage, nullptr);
	}

	ITextureHandle Device::CreatePlacedTexture(IMemoryHandle memory, uint64_t offset, TextureDesc const & textureDesc) {
		return MakeHandle<PlacedTexture>(dynamic_cast<Memory *>(memory.get()), offset, textureDesc);
	}

}