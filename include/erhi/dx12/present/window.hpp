#pragma once

#include "../../common/present/window.hpp"
#include "../native.hpp"

#include <WinUser.h>
#include <windef.h>

namespace erhi::dx12
{
	struct Window : IWindow {
		DeviceHandle mpDevice;
		WNDCLASSEXA mWindowClassInfo;
		ATOM mWindowClassAtom;
		HWND mWindowHandle;

		Window(DeviceHandle pDevice, WindowDesc const & desc, std::shared_ptr<IMessageCallback> pMessageCallback);
		virtual ~Window() override;

		virtual ISwapChainHandle CreateSwapChain(SwapChainDesc const & desc) override;
	};
}