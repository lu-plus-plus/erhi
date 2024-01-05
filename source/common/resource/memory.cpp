#include "erhi/common/resource/memory.hpp"

namespace erhi {

	IBuffer::IBuffer(BufferDesc const & desc) : mDesc(desc) {}
	IBuffer::~IBuffer() = default;

	ITexture::ITexture(TextureDesc const & desc) : mDesc(desc) {}
	ITexture::~ITexture() = default;

}