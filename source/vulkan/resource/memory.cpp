#include "erhi/vulkan/context/context.hpp"
#include "erhi/vulkan/resource/resource.hpp"

#include <format>		// for log formatting
#include <utility>		// for std::pair used in enum mapping
#include <bit>			// for bit manipulation in selecting memory type

#include "magic_enum_format.hpp"



namespace erhi::vk {

	Buffer::Buffer(Device * pDevice, MemoryHeapType heapType, BufferDesc const & bufferDesc) :
		IBuffer(bufferDesc), mpDevice(pDevice), mAllocation(VK_NULL_HANDLE), mBuffer(VK_NULL_HANDLE), mBufferDeviceAddress(0) {
		
		VkBufferCreateInfo const bufferCreateInfo = mapping::MapBufferCreateInfo(bufferDesc);
		VmaAllocationCreateInfo const allocationCreateInfo = mapping::MapHeapType(heapType);
		vkCheckResult(vmaCreateBuffer(pDevice->mAllocator, &bufferCreateInfo, &allocationCreateInfo, &mBuffer, &mAllocation, nullptr));

		VkBufferDeviceAddressInfo const addressInfo = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
			.pNext = nullptr,
			.buffer = mBuffer
		};
		mBufferDeviceAddress = vkGetBufferDeviceAddress(*pDevice, &addressInfo);
	}

	Buffer::~Buffer() {
		vmaDestroyBuffer(mpDevice->mAllocator, mBuffer, mAllocation);
	}



	IBufferHandle Device::CreateBuffer(MemoryHeapType heapType, BufferDesc const & bufferDesc) {
		return new Buffer(this, heapType, bufferDesc);
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
		if (flags & TextureUsageShaderResource) {
			result |= VK_IMAGE_USAGE_SAMPLED_BIT;
		}
		if (flags & (TextureUsageUnorderedAccess | TextureUsageShaderAtomic)) {
			result |= VK_IMAGE_USAGE_STORAGE_BIT;
		}
		if (flags & TextureUsageRenderTarget) {
			result |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		}
		if (flags & TextureUsageDepthStencil) {
			result |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		}
		return result;
	}

	static VkImageLayout MapTextureLayout(TextureLayout layout) {
		switch (layout) {
			case TextureLayout::Undefined: return VK_IMAGE_LAYOUT_UNDEFINED;
			case TextureLayout::Common: return VK_IMAGE_LAYOUT_GENERAL;
			case TextureLayout::Present: return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			case TextureLayout::ShaderResource: {
				return VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
			}
			case TextureLayout::UnorderedAccess: {
				return VK_IMAGE_LAYOUT_GENERAL;
			}
			case TextureLayout::RenderTarget: return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			case TextureLayout::DepthStencilWrite: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			case TextureLayout::DepthStencilRead: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			case TextureLayout::CopySource: return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			case TextureLayout::CopyTarget: return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		}
		return VK_IMAGE_LAYOUT_UNDEFINED;
	}

	static VkImageCreateInfo GetImageCreateInfo(TextureDesc const & desc) {
		return VkImageCreateInfo{
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.imageType = static_cast<VkImageType>(desc.dimension),
			.format = mapping::MapFormat(desc.format),
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
		VmaAllocationCreateInfo const allocationCreate = mapping::MapHeapType(heapType);
		vkCheckResult(vmaCreateImage(pDevice->mAllocator, &imageCreate, &allocationCreate, &mImage, &mAllocation, nullptr));
	}

	Texture::~Texture() {
		vmaDestroyImage(mpDevice->mAllocator, mImage, mAllocation);
	}



	ITextureHandle Device::CreateTexture(MemoryHeapType heapType, TextureDesc const & textureDesc) {
		return new Texture(this, heapType, textureDesc);
	}

}