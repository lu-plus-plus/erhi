#include "erhi/dx12/present/window.hpp"
#include "erhi/dx12/context/context.hpp"

#include <format>



namespace erhi::dx12
{
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	Window::Window(WindowDesc const & desc, std::shared_ptr<IMessageCallback> pMessageCallback) :
		IWindow(desc), mpMessageCallback(std::move(pMessageCallback)),
		mWindowClassInfo(), mWindowClassAtom(0), mWindowHandle(nullptr) {
		
		mWindowClassInfo = WNDCLASSEXA{
			.cbSize = sizeof(WNDCLASSEX),
			.style = CS_VREDRAW | CS_HREDRAW,
			.lpfnWndProc = WindowProc,
			.cbClsExtra = 0,
			.cbWndExtra = 0,
			.hInstance = GetModuleHandleA(nullptr),
			.hIcon = nullptr,
			.hCursor = LoadCursorA(nullptr, IDC_ARROW),
			.hbrBackground = nullptr,
			.lpszMenuName = nullptr,
			.lpszClassName = "eRHI Window Class",
			.hIconSm = nullptr
		};

		mWindowClassAtom = RegisterClassExA(&mWindowClassInfo);

		if (mWindowClassAtom == 0)
		{
			throw std::runtime_error("Failed to register a new window class in Windows.");
		}

		mWindowHandle = CreateWindowA(
			mWindowClassInfo.lpszClassName,
			desc.windowName,
			WS_OVERLAPPEDWINDOW,
			desc.left, desc.top,
			desc.width, desc.height,
			nullptr,
			nullptr,
			mWindowClassInfo.hInstance,
			this
		);

		if (mWindowHandle == nullptr)
		{
			throw std::runtime_error("Failed to create a new window in Windows.");
		}

		ShowWindow(mWindowHandle, SW_SHOWDEFAULT);
	}

	Window::~Window() {
		if (mWindowHandle) {
			if (0 == DestroyWindow(mWindowHandle)) {
				if (mpMessageCallback)
					mpMessageCallback->Error(std::format("fatal error in destroying window '{}'", mInitDesc.windowName));
				std::exit(EXIT_FAILURE);
			}
		}

		if (mWindowClassAtom) {
			if (0 == UnregisterClassA(mWindowClassInfo.lpszClassName, mWindowClassInfo.hInstance)) {
				if (mpMessageCallback)
					mpMessageCallback->Error(std::format("fatal error in unregistering window class '{}'", mWindowClassInfo.lpszClassName));
				std::exit(EXIT_FAILURE);
			}
		}
	}

	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_CREATE) {
			LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
		}
		else {
			auto pWindow = reinterpret_cast<Window *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

			switch (message) {
				case WM_PAINT:
					pWindow->mInitDesc.pMessageCallback.OnRender();
					break;
				default:
					/* do nothing */
					break;
			}
		}

		// Handle any message the switch statement skipped.
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	IWindowHandle Device::CreateNewWindow(WindowDesc const & desc) {
		return new Window(desc, mpMessageCallback);
	}
}