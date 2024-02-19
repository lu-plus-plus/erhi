#include <cstdio>

#include "erhi/vulkan/native.hpp"



namespace erhi::vk {

	char const * vkErrorCode(VkResult result) {
		#define STR(r) case VK_##r: return #r

		switch (result) {
			STR(NOT_READY);
			STR(TIMEOUT);
			STR(EVENT_SET);
			STR(EVENT_RESET);
			STR(INCOMPLETE);
			STR(ERROR_OUT_OF_HOST_MEMORY);
			STR(ERROR_OUT_OF_DEVICE_MEMORY);
			STR(ERROR_INITIALIZATION_FAILED);
			STR(ERROR_DEVICE_LOST);
			STR(ERROR_MEMORY_MAP_FAILED);
			STR(ERROR_LAYER_NOT_PRESENT);
			STR(ERROR_EXTENSION_NOT_PRESENT);
			STR(ERROR_FEATURE_NOT_PRESENT);
			STR(ERROR_INCOMPATIBLE_DRIVER);
			STR(ERROR_TOO_MANY_OBJECTS);
			STR(ERROR_FORMAT_NOT_SUPPORTED);
			STR(ERROR_SURFACE_LOST_KHR);
			STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
			STR(SUBOPTIMAL_KHR);
			STR(ERROR_OUT_OF_DATE_KHR);
			STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
			STR(ERROR_VALIDATION_FAILED_EXT);
			STR(ERROR_INVALID_SHADER_NV);
			default: return "UNKNOWN_ERROR";
		}

		#undef STR
	}

}



namespace erhi::vk::mapping {

	uint32_t MapHeapTypeToMemoryTypeBits(VkPhysicalDeviceMemoryProperties const & memoryProperties, MemoryHeapType heapType) {

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

		auto memoryTypeBits = 0u;

		for (auto i = 0u; i < memoryProperties.memoryTypeCount; ++i) {
			if (memoryProperties.memoryTypes[i].propertyFlags == propertyFlags) {
				memoryTypeBits |= 1 << i;
			}
		}

		return memoryTypeBits;
	}

	VmaAllocationCreateInfo MapHeapType(MemoryHeapType heapType) {
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

	VkBufferUsageFlags MapBufferUsage(BufferUsageFlags flags) {
		VkBufferUsageFlags result = 0;

		if (flags & BufferUsageCopySource) result |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		if (flags & BufferUsageCopyTarget) result |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		if ((flags & BufferUsageShaderResource) | (flags & BufferUsageUnorderedAccess)) result |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		if (flags & BufferUsageConstantBuffer) result |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		if (flags & BufferUsageIndexBuffer) result |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		if (flags & BufferUsageVertexBuffer) result |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		// if (flags & BufferUsageShaderAtomic) do nothing;

		return result;
	}

	VkBufferCreateInfo MapBufferCreateInfo(BufferDesc const & desc) {
		VkBufferCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.size = desc.size,
			.usage = MapBufferUsage(desc.usage),
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = /* ignored */ 0,
			.pQueueFamilyIndices = /* ignored */ nullptr
		};

		// always keep device address available
		createInfo.usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

		return createInfo;
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

			case Format::B8G8R8A8_UNorm:
				return VK_FORMAT_B8G8R8A8_UNORM;
			case Format::B8G8R8A8_UNormSRGB:
				return VK_FORMAT_B8G8R8A8_SRGB;

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

	VkDescriptorType MapDescriptorType(DescriptorType type) {
		switch (type)
		{
			case erhi::DescriptorType::Sampler:
				return VK_DESCRIPTOR_TYPE_SAMPLER;
				break;
			case erhi::DescriptorType::BufferShaderResource:
				return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				break;
			case erhi::DescriptorType::BufferUnorderedAccess:
				return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				break;
			case erhi::DescriptorType::BufferConstantBuffer:
				return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				break;
			case erhi::DescriptorType::TextureShaderResource:
				return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
				break;
			case erhi::DescriptorType::TextureUnorderedAccess:
				return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
				break;
			default:
				return VK_DESCRIPTOR_TYPE_MAX_ENUM;
				break;
		}
	}

	VkShaderStageFlags MapShaderStageFlags(ShaderStageFlags flags) {
		VkShaderStageFlags result = 0;
		if (flags & ShaderStageVertex) result |= VK_SHADER_STAGE_VERTEX_BIT;
		if (flags & ShaderStagePixel) result |= VK_SHADER_STAGE_FRAGMENT_BIT;
		if (flags & ShaderStageCompute) result |= VK_SHADER_STAGE_COMPUTE_BIT;
		if (flags & ShaderStageAllGraphics) result |= VK_SHADER_STAGE_ALL_GRAPHICS;
		if (flags & ShaderStageAll) result |= VK_SHADER_STAGE_ALL;
		return result;
	}

	VkSampleCountFlagBits MapTextureSampleCount(TextureSampleCount sampleCount) {
		return static_cast<VkSampleCountFlagBits>(1u << static_cast<uint32_t>(sampleCount));
	}

	VkImageUsageFlags MapTextureUsage(TextureUsageFlags flags) {
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

	VkImageLayout MapTextureLayout(TextureLayout layout) {
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

	VkAttachmentLoadOp MapAttachmentLoadOp(AttachmentLoadOp loadOp) {
		switch (loadOp)
		{
			case erhi::AttachmentLoadOp::Load:
				return VK_ATTACHMENT_LOAD_OP_LOAD;
				break;
			case erhi::AttachmentLoadOp::Clear:
				return VK_ATTACHMENT_LOAD_OP_CLEAR;
				break;
			case erhi::AttachmentLoadOp::DoNotCare:
			default:
				return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				break;
		}
	}

	VkAttachmentStoreOp MapAttachmentStoreOp(AttachmentStoreOp storeOp) {
		switch (storeOp)
		{
			case erhi::AttachmentStoreOp::Store:
				return VK_ATTACHMENT_STORE_OP_STORE;
				break;
			case erhi::AttachmentStoreOp::DoNotCare:
			default:
				return VK_ATTACHMENT_STORE_OP_DONT_CARE;
				break;
		}
	}

	VkPipelineBindPoint MapPipelineBindPoint(PipelineBindPoint bindPoint) {
		switch (bindPoint)
		{
			case erhi::PipelineBindPoint::Graphics:
			default:
				return VK_PIPELINE_BIND_POINT_GRAPHICS;
				break;
			case erhi::PipelineBindPoint::Compute:
				return VK_PIPELINE_BIND_POINT_COMPUTE;
				break;
		}
	}

}