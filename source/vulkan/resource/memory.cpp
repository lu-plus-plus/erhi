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
			.samples = mapping::MapTextureSampleCount(desc.sampleCount),
			.tiling = static_cast<VkImageTiling>(desc.tiling),
			.usage = mapping::MapTextureUsage(desc.usage),
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
		};
	}



	Texture::Texture(DeviceHandle pDevice, MemoryHeapType heapType, TextureDesc const & textureDesc) : ITexture(textureDesc), mpDevice(pDevice), mAllocation(VK_NULL_HANDLE), mImage(VK_NULL_HANDLE) {
		VkImageCreateInfo const imageCreate = GetImageCreateInfo(textureDesc);
		VmaAllocationCreateInfo const allocationCreate = mapping::MapHeapType(heapType);
		vkCheckResult(vmaCreateImage(pDevice->mAllocator, &imageCreate, &allocationCreate, &mImage, &mAllocation, nullptr));
	}

	Texture::Texture(DeviceHandle pDevice, VkImage image, TextureDesc const & desc) : ITexture(desc), mpDevice(pDevice), mAllocation(VK_NULL_HANDLE), mImage(image) {}

	Texture::~Texture() {
		if (mAllocation and mImage) {
			vmaDestroyImage(mpDevice->mAllocator, mImage, mAllocation);
		}
	}



	ITextureHandle Device::CreateTexture(MemoryHeapType heapType, TextureDesc const & textureDesc) {
		return new Texture(this, heapType, textureDesc);
	}

}