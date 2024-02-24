#pragma once

#include "../common.hpp"

#include <vector>
#include <optional>

namespace erhi {

	struct IRenderPass {
		PipelineBindPoint mPipelineBindPoint;
		std::vector<AttachmentDesc> mAttachments;
		std::vector<uint32_t> mRenderTargetAttachments;
		std::optional<uint32_t> mDepthStencilAttachment;

		IRenderPass(RenderPassDesc const & desc);
		virtual ~IRenderPass() = 0;
	};

}