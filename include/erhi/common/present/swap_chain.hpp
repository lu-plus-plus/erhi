#include "../common.hpp"

namespace erhi {
	struct ISwapChain {
		SwapChainDesc mDesc;

		ISwapChain(SwapChainDesc const & desc);
		virtual ~ISwapChain() = 0;

		virtual ITextureHandle GetTexture(uint32_t index) = 0;
	};
}