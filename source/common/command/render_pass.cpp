#include "erhi/common/command/render_pass.hpp"

namespace erhi
{
	IRenderPass::IRenderPass(RenderPassDesc const & desc) : mDesc(desc) {}
	IRenderPass::~IRenderPass() = default;
}