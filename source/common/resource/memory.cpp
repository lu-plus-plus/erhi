#include "erhi/common/resource/memory.hpp"

namespace erhi {
	
	IMemory::IMemory(MemoryDesc const & desc) : mDesc(desc) {}

	IMemory::~IMemory() = default;



	IBuffer::IBuffer(BufferDesc const & desc) : mDesc(desc) {}

	IBuffer::~IBuffer() = default;



	ITexture::ITexture(TextureDesc const & desc) : mDesc(desc) {}

	ITexture::~ITexture() = default;

}