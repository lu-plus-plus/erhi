#include "erhi/common/resource/memory.hpp"

namespace erhi {
	
	IMemory::IMemory(MemoryRequirements const & requirements) : mRequirements(requirements) {}
	IMemory::~IMemory() = default;



	IBuffer::IBuffer(BufferDesc const & desc) : mDesc(desc) {}
	IBuffer::~IBuffer() = default;



	ITexture::ITexture(TextureDesc const & desc) : mDesc(desc) {}
	ITexture::~ITexture() = default;

}