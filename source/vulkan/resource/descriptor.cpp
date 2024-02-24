#include "erhi/vulkan/context/context.hpp"
#include "erhi/vulkan/resource/resource.hpp"

#include <cassert>



namespace erhi::vk
{
	VkImageViewType MapTextureViewDimension(TextureViewDimension dimension) {
		switch (dimension)
		{
			case erhi::TextureViewDimension::Texture1D:
				return VK_IMAGE_VIEW_TYPE_1D;
				break;
			case erhi::TextureViewDimension::Texture2D:
				return VK_IMAGE_VIEW_TYPE_2D;
				break;
			case erhi::TextureViewDimension::Texture3D:
				return VK_IMAGE_VIEW_TYPE_3D;
				break;
			default:
				return VK_IMAGE_VIEW_TYPE_1D;
				break;
		}
	}

	VkImageAspectFlags MapTextureAspectFlags(TextureAspectFlags flags) {
		VkImageAspectFlags result = 0;
		if (flags & TextureAspectColor) result |= VK_IMAGE_ASPECT_COLOR_BIT;
		if (flags & TextureAspectDepth) result |= VK_IMAGE_ASPECT_DEPTH_BIT;
		if (flags & TextureAspectStencil) result |= VK_IMAGE_ASPECT_STENCIL_BIT;
		return result;
	}



	TextureView::TextureView(DeviceHandle pDevice, TextureHandle pTexture, TextureViewDesc const & desc) : ITextureView(desc), mpDevice(pDevice), mImageView(VK_NULL_HANDLE) {
		VkImageViewCreateInfo const createInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.image = pTexture->mImage,
			.viewType = MapTextureViewDimension(desc.dimension),
			.format = mapping::MapFormat(desc.format),
			.components = VkComponentMapping{
				.r = VK_COMPONENT_SWIZZLE_IDENTITY,
				.g = VK_COMPONENT_SWIZZLE_IDENTITY,
				.b = VK_COMPONENT_SWIZZLE_IDENTITY,
				.a = VK_COMPONENT_SWIZZLE_IDENTITY
			},
			.subresourceRange = VkImageSubresourceRange{
				.aspectMask = MapTextureAspectFlags(desc.aspectFlags),
				.baseMipLevel = desc.mostDetailedMipLevel,
				.levelCount = desc.mipLevelCount,
				.baseArrayLayer = 0,
				.layerCount = VK_REMAINING_ARRAY_LAYERS
			}
		};
		vkCheckResult(vkCreateImageView(*pDevice, &createInfo, nullptr, &mImageView));
	}

	TextureView::~TextureView() {
		vkDestroyImageView(*mpDevice, mImageView, nullptr);
	}

	ITextureViewHandle Device::CreateTextureView(ITextureHandle pTexture, TextureViewDesc const & desc) {
		return new TextureView(this, dynamic_cast<TextureHandle>(pTexture), desc);
	}



	CPUDescriptorHeap::CPUDescriptorHeap(DeviceHandle pDevice, DescriptorHeapDesc const & desc) :
		ICPUDescriptorHeap(desc), mpDevice(pDevice), mDescriptorBuffer(VK_NULL_HANDLE), mDescriptorBufferAllocation(VK_NULL_HANDLE), mHostMapping(nullptr) {
		
		VkBufferCreateInfo descriptorBufferCreateInfo = mapping::MapBufferCreateInfo(BufferDesc{
			.usage = 0,
			.size = desc.sizeInBytes
		});

		if (desc.type == DescriptorHeapType::CBV_SRV_UAV)
			descriptorBufferCreateInfo.usage |= VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT;
		else if (desc.type == DescriptorHeapType::Sampler)
			descriptorBufferCreateInfo.usage |= VK_BUFFER_USAGE_SAMPLER_DESCRIPTOR_BUFFER_BIT_EXT;

		VmaAllocationCreateInfo const allocationInfo = mapping::MapHeapType(MemoryHeapType::Upload);

		vkCheckResult(vmaCreateBuffer(pDevice->mAllocator, &descriptorBufferCreateInfo, &allocationInfo, &mDescriptorBuffer, &mDescriptorBufferAllocation, nullptr));

		void * ptr = nullptr;
		vkCheckResult(vmaMapMemory(pDevice->mAllocator, mDescriptorBufferAllocation, &ptr));
		mHostMapping = static_cast<std::byte *>(ptr);
	}

	CPUDescriptorHeap::~CPUDescriptorHeap() {
		vmaUnmapMemory(mpDevice->mAllocator, mDescriptorBufferAllocation);
		vmaDestroyBuffer(mpDevice->mAllocator, mDescriptorBuffer, mDescriptorBufferAllocation);
	}

	ICPUDescriptorHeapHandle Device::CreateCPUDescriptorHeap(DescriptorHeapDesc const & desc) {
		return new CPUDescriptorHeap(this, desc);
	}



	GPUDescriptorHeap::GPUDescriptorHeap(DeviceHandle pDevice, DescriptorHeapDesc const & desc) : IGPUDescriptorHeap(desc), mpDevice(pDevice), mDescriptorBuffer(VK_NULL_HANDLE), mDescriptorBufferAllocation(VK_NULL_HANDLE) {
		VkBufferCreateInfo descriptorBufferCreateInfo = mapping::MapBufferCreateInfo(BufferDesc{
			.usage = 0,
			.size = desc.sizeInBytes
		});

		if (desc.type == DescriptorHeapType::CBV_SRV_UAV)
			descriptorBufferCreateInfo.usage |= VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT;
		else if (desc.type == DescriptorHeapType::Sampler)
			descriptorBufferCreateInfo.usage |= VK_BUFFER_USAGE_SAMPLER_DESCRIPTOR_BUFFER_BIT_EXT;

		VmaAllocationCreateInfo const allocationInfo = mapping::MapHeapType(MemoryHeapType::Default);

		vkCheckResult(vmaCreateBuffer(pDevice->mAllocator, &descriptorBufferCreateInfo, &allocationInfo, &mDescriptorBuffer, &mDescriptorBufferAllocation, nullptr));
	}

	GPUDescriptorHeap::~GPUDescriptorHeap() {
		vmaDestroyBuffer(mpDevice->mAllocator, mDescriptorBuffer, mDescriptorBufferAllocation);
	}

	IGPUDescriptorHeapHandle Device::CreateGPUDescriptorHeap(DescriptorHeapDesc const & desc) {
		return new GPUDescriptorHeap(this, desc);
	}



	DescriptorSetLayout::DescriptorSetLayout(Device * pDevice, DescriptorSetLayoutDesc const & desc) : IDescriptorSetLayout(desc), mpDevice(pDevice), mDescriptorSetLayout(VK_NULL_HANDLE) {
		std::vector<VkDescriptorSetLayoutBinding> bindings(desc.bindingCount);

		for (size_t i = 0; i < desc.bindingCount; ++i) {
			auto & output = bindings[i];
			auto const & input = desc.bindings[i];

			output.binding = input.firstRegister;
			output.descriptorType = mapping::MapDescriptorType(input.descriptorType);
			output.descriptorCount = input.descriptorCount;
			output.stageFlags = mapping::MapShaderStageFlags(input.shaderStageFlags);
			output.pImmutableSamplers = nullptr;
		}

		VkDescriptorSetLayoutCreateInfo const layoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_DESCRIPTOR_BUFFER_BIT_EXT,
			.bindingCount = desc.bindingCount,
			.pBindings = bindings.data()
		};

		vkCheckResult(vkCreateDescriptorSetLayout(*pDevice, &layoutCreateInfo, nullptr, &mDescriptorSetLayout));
	}

	DescriptorSetLayout::~DescriptorSetLayout() {
		vkDestroyDescriptorSetLayout(*mpDevice, mDescriptorSetLayout, nullptr);
	}

	IDescriptorSetLayoutHandle Device::CreateDescriptorSetLayout(DescriptorSetLayoutDesc const & desc) {
		return new DescriptorSetLayout(this, desc);
	}



	uint64_t Device::GetDescriptorSetLayoutSize(IDescriptorSetLayoutHandle pLayout) {
		VkDeviceSize size = 0;
		vkGetDescriptorSetLayoutSizeEXT(mDevice, dynamic_cast<DescriptorSetLayoutHandle>(pLayout)->mDescriptorSetLayout, &size);
		return size;
	}

	uint64_t Device::GetDescriptorSetLayoutBindingOffset(IDescriptorSetLayoutHandle pLayout, uint64_t binding) {
		VkDeviceSize offset = 0;
		vkGetDescriptorSetLayoutBindingOffsetEXT(mDevice, dynamic_cast<DescriptorSetLayoutHandle>(pLayout)->mDescriptorSetLayout, binding, &offset);
		return offset;
	}



	void CPUDescriptorHeap::CreateBufferShaderResourceView(uint64_t offsetInBytes, IBufferHandle pBuffer, BufferDescriptorDesc const & desc) {
		assert(not (desc.format != Format::Unknown and desc.structureSizeInBytes != 0));

		uint64_t const elementStride = desc.format != Format::Unknown
			? mpDevice->mGlobalConstants.FormatSizeInBytes[static_cast<size_t>(desc.format)]
			: desc.structureSizeInBytes;

		VkDescriptorAddressInfoEXT const addressInfo{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_ADDRESS_INFO_EXT,
			.pNext = nullptr,
			.address = dynamic_cast<BufferHandle>(pBuffer)->mBufferDeviceAddress + desc.offsetInElements * elementStride,
			.range = desc.countInElements * elementStride,
			.format = VK_FORMAT_UNDEFINED /* only for texel buffer */
		};

		VkDescriptorGetInfoEXT const getInfo{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_GET_INFO_EXT,
			.pNext = nullptr,
			.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			.data = VkDescriptorDataEXT{ .pStorageBuffer = &addressInfo }
		};

		uint64_t const dataSize = mpDevice->mpPhysicalDevice->DescriptorSizeInBytes(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);

		vkGetDescriptorEXT(*mpDevice, &getInfo, dataSize, mHostMapping + offsetInBytes);
	}

	void CPUDescriptorHeap::CreateBufferUnorderedAccessView(uint64_t offsetInBytes, IBufferHandle pBuffer, BufferDescriptorDesc const & desc) {
		CreateBufferShaderResourceView(offsetInBytes, pBuffer, desc);
	}

	void CPUDescriptorHeap::CreateTextureShaderResourceView(uint64_t offsetInBytes, ITextureViewHandle pTextureView) {
		VkDescriptorImageInfo const imageInfo{
			.sampler = VK_NULL_HANDLE,
			.imageView = dynamic_cast<TextureViewHandle>(pTextureView)->mImageView,
			.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		};

		VkDescriptorGetInfoEXT const getInfo{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_GET_INFO_EXT,
			.pNext = nullptr,
			.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
			.data = VkDescriptorDataEXT{ .pSampledImage = &imageInfo }
		};

		uint64_t const dataSize = mpDevice->mpPhysicalDevice->DescriptorSizeInBytes(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);

		vkGetDescriptorEXT(*mpDevice, &getInfo, dataSize, mHostMapping + offsetInBytes);
	}

	void CPUDescriptorHeap::CreateTextureUnorderedAccessView(uint64_t offsetInBytes, ITextureViewHandle pTextureView) {
		VkDescriptorImageInfo const imageInfo{
			.sampler = VK_NULL_HANDLE,
			.imageView = dynamic_cast<TextureViewHandle>(pTextureView)->mImageView,
			.imageLayout = VK_IMAGE_LAYOUT_GENERAL
		};

		VkDescriptorGetInfoEXT const getInfo{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_GET_INFO_EXT,
			.pNext = nullptr,
			.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
			.data = VkDescriptorDataEXT{ .pStorageImage = &imageInfo }
		};

		uint64_t const dataSize = mpDevice->mpPhysicalDevice->DescriptorSizeInBytes(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);

		vkGetDescriptorEXT(*mpDevice, &getInfo, dataSize, mHostMapping + offsetInBytes);
	}

}