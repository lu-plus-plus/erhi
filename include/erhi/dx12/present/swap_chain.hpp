#pragma once

#include "erhi/common/present/swap_chain.hpp"
#include "../native.hpp"

namespace erhi::dx12 {

	struct SwapChain : ISwapChain {
		DeviceHandle mpDevice;
		WindowHandle mpWindow;
		ComPtr<IDXGISwapChainLatest> mpSwapChainLatest;

		SwapChain(DeviceHandle pDevice, WindowHandle pWindow, SwapChainDesc const & desc);
		virtual ~SwapChain() override;
	};

}