#include "erhi/vulkan/context/context.hpp"
#include "erhi/vulkan/resource/resource.hpp"

#include <format>		// for log formatting
#include <utility>		// for std::pair used in enum mapping
#include <bit>			// for bit manipulation in selecting memory type

#include "magic_enum_format.hpp"



namespace erhi::vk {

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

		auto const & memoryProperties = pDevice->mPhysicalDevice.mMemoryProperties.memoryProperties;

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
			{ BufferUsageShaderResource, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT },
			{ BufferUsageUnorderedAccess, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT },
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



	Buffer::Buffer(Device * pDevice, MemoryHeapType heapType, BufferDesc const & bufferDesc) : IBuffer(bufferDesc), mpDevice(pDevice), mAllocation(VK_NULL_HANDLE), mBuffer(VK_NULL_HANDLE) {
		VkBufferCreateInfo const bufferCreateInfo = GetBufferCreateInfo(bufferDesc);
		VmaAllocationCreateInfo const allocationCreateInfo = MapHeapType(heapType);
		vkCheckResult(vmaCreateBuffer(pDevice->mAllocator, &bufferCreateInfo, &allocationCreateInfo, &mBuffer, &mAllocation, nullptr));
	}

	Buffer::~Buffer() {
		vmaDestroyBuffer(mpDevice->mAllocator, mBuffer, mAllocation);
	}



	IBufferHandle Device::CreateBuffer(MemoryHeapType heapType, BufferDesc const & bufferDesc) {
		return new Buffer(this, heapType, bufferDesc);
	}



	VkFormat MapFormat(Format format) {
		switch (format) {
			case Format::Unknown:
				return VK_FORMAT_UNDEFINED;

			case Format::R32G32B32A32_Typeless:
			case Format::R32G32B32A32_Float:
				return VK_FORMAT_R32G32B32A32_SFLOAT;
			case Format::R32G32B32A32_UInt:
				return VK_FORMAT_R32G32B32A32_UINT;
			case Format::R32G32B32A32_SInt:
				return VK_FORMAT_R32G32B32A32_SINT;
			
			case Format::R32G32B32_Typeless:
			case Format::R32G32B32_Float:
				return VK_FORMAT_R32G32B32_SFLOAT;
			case Format::R32G32B32_UInt:
				return VK_FORMAT_R32G32B32_UINT;
			case Format::R32G32B32_SInt:
				return VK_FORMAT_R32G32B32_SINT;

			case Format::R16G16B16A16_Typeless:
			case Format::R16G16B16A16_Float:
				return VK_FORMAT_R16G16B16A16_SFLOAT;
			case Format::R16G16B16A16_UNorm:
				return VK_FORMAT_R16G16B16A16_UNORM;
			case Format::R16G16B16A16_UInt:
				return VK_FORMAT_R16G16B16A16_UINT;
			case Format::R16G16B16A16_SNorm:
				return VK_FORMAT_R16G16B16A16_SNORM;
			case Format::R16G16B16A16_SInt:
				return VK_FORMAT_R16G16B16A16_SINT;

			case Format::R32G32_Typeless:
			case Format::R32G32_Float:
				return VK_FORMAT_R32G32_SFLOAT;
			case Format::R32G32_UInt:
				return VK_FORMAT_R32G32_UINT;
			case Format::R32G32_SInt:
				return VK_FORMAT_R32G32_SINT;
			
			case Format::R8G8B8A8_Typeless:
			case Format::R8G8B8A8_UNorm:
				return VK_FORMAT_R8G8B8A8_UNORM;
			case Format::R8G8B8A8_UInt:
				return VK_FORMAT_R8G8B8A8_UINT;
			case Format::R8G8B8A8_SNorm:
				return VK_FORMAT_R8G8B8A8_SNORM;
			case Format::R8G8B8A8_SInt:
				return VK_FORMAT_R8G8B8A8_SINT;
			
			case Format::R16G16_Typeless:
			case Format::R16G16_Float:
				return VK_FORMAT_R16G16_SFLOAT;
			case Format::R16G16_UNorm:
				return VK_FORMAT_R16G16_UNORM;
			case Format::R16G16_UInt:
				return VK_FORMAT_R16G16_UINT;
			case Format::R16G16_SNorm:
				return VK_FORMAT_R16G16_SNORM;
			case Format::R16G16_SInt:
				return VK_FORMAT_R16G16_SINT;
			
			case Format::R32_Float:
				return VK_FORMAT_R32_SFLOAT;
			case Format::R32_UInt:
				return VK_FORMAT_R32_UINT;
			case Format::R32_SInt:
				return VK_FORMAT_R32_SINT;
			
			case Format::D32_Float:
				return VK_FORMAT_D32_SFLOAT;
			case Format::D16_UNorm:
				return VK_FORMAT_D16_UNORM;
			case Format::D24_UNorm_S8_UInt:
				return VK_FORMAT_D24_UNORM_S8_UINT;
		}

		return VK_FORMAT_UNDEFINED;
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



	Texture::Texture(Device * pDevice, MemoryHeapType heapType, TextureDesc const & textureDesc) : ITexture(textureDesc), mpDevice(pDevice), mAllocation(VK_NULL_HANDLE), mImage(VK_NULL_HANDLE) {
		VkImageCreateInfo const imageCreate = GetImageCreateInfo(textureDesc);
		VmaAllocationCreateInfo const allocationCreate = MapHeapType(heapType);
		vkCheckResult(vmaCreateImage(pDevice->mAllocator, &imageCreate, &allocationCreate, &mImage, &mAllocation, nullptr));
	}

	Texture::~Texture() {
		vmaDestroyImage(mpDevice->mAllocator, mImage, mAllocation);
	}



	ITextureHandle Device::CreateTexture(MemoryHeapType heapType, TextureDesc const & textureDesc) {
		return new Texture(this, heapType, textureDesc);
	}

}