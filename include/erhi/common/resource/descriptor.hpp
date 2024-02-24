#pragma once

#include "../common.hpp"

#include <vector>



namespace erhi {

	struct ITextureView {
		TextureViewDesc mDesc;

		ITextureView(TextureViewDesc const & desc);
		virtual ~ITextureView() = 0;
	};

	struct ICPUDescriptorHeap {
		DescriptorHeapDesc mDesc;

		ICPUDescriptorHeap(DescriptorHeapDesc const & desc);
		virtual ~ICPUDescriptorHeap() = 0;

		virtual void CreateBufferShaderResourceView(uint64_t offsetInBytes, IBufferHandle pBuffer, BufferDescriptorDesc const & desc) = 0;
		virtual void CreateBufferUnorderedAccessView(uint64_t offsetInBytes, IBufferHandle pBuffer, BufferDescriptorDesc const & desc) = 0;
		virtual void CreateTextureShaderResourceView(uint64_t offsetInBytes, ITextureViewHandle pTextureView) = 0;
		virtual void CreateTextureUnorderedAccessView(uint64_t offsetInBytes, ITextureViewHandle pTextureView) = 0;
	};

	struct IGPUDescriptorHeap {
		DescriptorHeapDesc mDesc;

		IGPUDescriptorHeap(DescriptorHeapDesc const & desc);
		virtual ~IGPUDescriptorHeap() = 0;
	};

	struct IDescriptorSetLayout {
		std::vector<DescriptorSetLayoutBinding> mBindings;
		DescriptorHeapType mDescriptorHeapType;

		IDescriptorSetLayout(DescriptorSetLayoutDesc const & desc);
		virtual ~IDescriptorSetLayout() = 0;
	};

}