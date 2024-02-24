#pragma once

#include "erhi/common/command/render_pass.hpp"
#include "../native.hpp"

namespace erhi::dx12 {
	struct RenderPass : IRenderPass {
		RenderPass(RenderPassDesc const & desc);
		virtual ~RenderPass() override;
	};
}