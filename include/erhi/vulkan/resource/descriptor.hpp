#pragma once

#include "../../common/resource/descriptor.hpp"
#include "../native.hpp"

#include <cstddef>		// for std::byte

namespace erhi::vk {

	struct TextureView : ITextureView {
		DeviceHandle mpDevice;
		VkImageView mImageView;

		TextureView(DeviceHandle pDevice, TextureHandle texture, TextureViewDesc const & desc);
		virtual ~TextureView() override;
	};

	struct CPUDescriptorHeap : ICPUDescriptorHeap {
		DeviceHandle mpDevice;
		VkBuffer mDescriptorBuffer;
		VmaAllocation mDescriptorBufferAllocation;
		std::byte * mHostMapping;

		CPUDescriptorHeap(DeviceHandle pDevice, DescriptorHeapDesc const & desc);
		virtual ~CPUDescriptorHeap() override;

		virtual void CreateBufferShaderResourceView(uint64_t offsetInBytes, IBufferHandle pBuffer, BufferDescriptorDesc const & desc) override;
		virtual void CreateBufferUnorderedAccessView(uint64_t offsetInBytes, IBufferHandle pBuffer, BufferDescriptorDesc const & desc) override;
		virtual void CreateTextureShaderResourceView(uint64_t offsetInBytes, ITextureViewHandle pTextureView) override;
		virtual void CreateTextureUnorderedAccessView(uint64_t offsetInBytes, ITextureViewHandle pTextureView) override;
	};

	struct GPUDescriptorHeap : IGPUDescriptorHeap {
		DeviceHandle mpDevice;
		VkBuffer mDescriptorBuffer;
		VmaAllocation mDescriptorBufferAllocation;

		GPUDescriptorHeap(DeviceHandle pDevice, DescriptorHeapDesc const & desc);
		virtual ~GPUDescriptorHeap() override;
	};

	struct DescriptorSetLayout : IDescriptorSetLayout {
		DeviceHandle mpDevice;
		VkDescriptorSetLayout mDescriptorSetLayout;

		DescriptorSetLayout(Device * pDevice, DescriptorSetLayoutDesc const & desc);
		virtual ~DescriptorSetLayout() override;
	};

}