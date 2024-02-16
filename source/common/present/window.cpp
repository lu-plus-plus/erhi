#include "erhi/common/present/window.hpp"

namespace erhi
{
	IWindow::IWindow(WindowDesc const & desc) : mInitDesc(desc), mCurrentWidth(desc.width), mCurrentHeight(desc.height) {}
	IWindow::~IWindow() = default;
}