#pragma once

#include "../common.hpp"

namespace erhi
{
	struct IWindow {
		WindowDesc mInitDesc;
		uint32_t mCurrentWidth;
		uint32_t mCurrentHeight;

		IWindow(WindowDesc const & desc);
		virtual ~IWindow() = 0;

		virtual ISwapChainHandle CreateSwapChain(SwapChainDesc const & desc) = 0;
	};
}