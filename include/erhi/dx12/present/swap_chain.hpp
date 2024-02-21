#pragma once

#include "erhi/common/present/swap_chain.hpp"
#include "../native.hpp"
#include "../resource/texture.hpp"



namespace erhi::dx12 {

	struct SwapChain : ISwapChain {
		DeviceHandle mpDevice;
		WindowHandle mpWindow;
		ComPtr<IDXGISwapChainLatest> mpSwapChainLatest;
		std::vector<TextureHandle> mSwapChainTextures;

		SwapChain(DeviceHandle pDevice, WindowHandle pWindow, SwapChainDesc const & desc);
		virtual ~SwapChain() override;

		virtual ITextureHandle GetTexture(uint32_t index) override;
	};

}