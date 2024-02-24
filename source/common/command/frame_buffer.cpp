#include "erhi/common/command/command.hpp"

namespace erhi {
	
	IFrameBuffer::IFrameBuffer(FrameBufferDesc const & desc) : mpRenderPass(desc.pRenderPass), mAttachments(desc.attachments, desc.attachments + desc.attachmentCount), mWidth(desc.width), mHeight(desc.height) {}
	IFrameBuffer::~IFrameBuffer() = default;

}