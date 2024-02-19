#include "../common.hpp"

namespace erhi {
	struct ISwapChain {
		SwapChainDesc mDesc;

		ISwapChain(SwapChainDesc const & desc);
		virtual ~ISwapChain() = 0;
	};
}