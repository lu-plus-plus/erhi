#pragma once

#include "../../common/present/window.hpp"
#include "../native.hpp"

#include <WinUser.h>
#include <windef.h>

namespace erhi::dx12
{
	struct Window : IWindow {
		std::shared_ptr<IMessageCallback> mpMessageCallback;

		WNDCLASSEXA mWindowClassInfo;
		ATOM mWindowClassAtom;
		HWND mWindowHandle;

		Window(WindowDesc const & desc, std::shared_ptr<IMessageCallback> pMessageCallback);
		virtual ~Window() override;
	};
}