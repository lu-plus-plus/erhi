#pragma once

#include "../../common/resource/memory.hpp"
#include "../native.hpp"



namespace erhi::vk {

	struct Buffer : public IBuffer {
		Device *			mpDevice;
		VmaAllocation		mAllocation;
		VkBuffer			mBuffer;
		VkDeviceAddress		mBufferDeviceAddress;

		Buffer(Device * pDevice, MemoryHeapType heapType, BufferDesc const & bufferDesc);
		virtual ~Buffer() override;
	};

	struct Texture : public ITexture {
		Device *			mpDevice;
		VmaAllocation		mAllocation;
		VkImage				mImage;

		Texture(Device * pDevice, MemoryHeapType heapType, TextureDesc const & desc);
		virtual ~Texture() override;
	};

}