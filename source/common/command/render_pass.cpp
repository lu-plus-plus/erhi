#include "erhi/common/command/render_pass.hpp"

#include <iterator>

namespace erhi
{
	IRenderPass::IRenderPass(RenderPassDesc const & desc) : mPipelineBindPoint(desc.pipelineBindPoint) {
		std::copy(desc.attachments, desc.attachments + desc.attachmentCount, std::back_inserter(mAttachments));
		std::copy(desc.renderTargetAttachments, desc.renderTargetAttachments + desc.renderTargetAttachmentCount, std::back_inserter(mRenderTargetAttachments));
		if (desc.pDepthStencilAttachment) {
			mDepthStencilAttachment = *desc.pDepthStencilAttachment;
		}
	}

	IRenderPass::~IRenderPass() = default;
}