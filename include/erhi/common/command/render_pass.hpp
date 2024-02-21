#pragma once

#include "../common.hpp"

namespace erhi {

	struct IRenderPass {
		RenderPassDesc mDesc;

		IRenderPass(RenderPassDesc const & desc);
		virtual ~IRenderPass() = 0;
	};

}