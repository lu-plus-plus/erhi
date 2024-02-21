#include "erhi/common/command/frame_buffer.hpp"

namespace erhi {
	
	IFrameBuffer::IFrameBuffer(FrameBufferDesc const & desc) : mDesc(desc) {}
	IFrameBuffer::~IFrameBuffer() = default;

}