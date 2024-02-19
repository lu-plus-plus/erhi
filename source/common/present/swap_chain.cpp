#include "erhi/common/present/swap_chain.hpp"

namespace erhi
{
	ISwapChain::ISwapChain(SwapChainDesc const & desc) : mDesc(desc) {}
	ISwapChain::~ISwapChain() = default;
}