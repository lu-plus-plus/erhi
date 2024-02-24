#include "erhi/dx12/context/context.hpp"
#include "erhi/dx12/command/command.hpp"

namespace erhi::dx12 {

	RenderPass::RenderPass(RenderPassDesc const & desc) : IRenderPass(desc) {}
	RenderPass::~RenderPass() = default;

	IRenderPassHandle Device::CreateRenderPass(RenderPassDesc const & desc) {
		return new RenderPass(desc);
	}

}