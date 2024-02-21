#include "erhi/common/present/swap_chain.hpp"
#include "../native.hpp"
#include "../resource/descriptor.hpp"

namespace erhi::vk {

	struct SwapChain : ISwapChain {
		DeviceHandle mpDevice;
		WindowHandle mpWindow;
		VkSwapchainKHR mSwapChain;
		std::vector<TextureHandle> mImages;

		SwapChain(DeviceHandle pDevice, WindowHandle pWindow, SwapChainDesc const & desc);
		virtual ~SwapChain() override;

		virtual ITextureHandle GetTexture(uint32_t index) override;
	};

}