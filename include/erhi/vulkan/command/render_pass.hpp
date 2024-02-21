#pragma once

#include "../../common/command/render_pass.hpp"
#include "../native.hpp"

namespace erhi::vk {

	struct RenderPass : IRenderPass {
		DeviceHandle mpDevice;
		VkRenderPass mRenderPass;

		RenderPass(DeviceHandle pDevice, RenderPassDesc const & desc);
		virtual ~RenderPass() override;
	};

}