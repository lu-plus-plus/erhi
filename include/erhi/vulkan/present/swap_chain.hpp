#include "erhi/common/present/swap_chain.hpp"
#include "../native.hpp"

namespace erhi::vk {

	struct SwapChain : ISwapChain {
		DeviceHandle mpDevice;
		WindowHandle mpWindow;
		VkSwapchainKHR mSwapChain;

		SwapChain(DeviceHandle pDevice, WindowHandle pWindow, SwapChainDesc const & desc);
		virtual ~SwapChain() override;
	};

}