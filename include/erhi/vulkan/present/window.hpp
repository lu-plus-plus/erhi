#pragma once

#include "../../common/present/window.hpp"
#include "../native.hpp"

// defining GLFW_INCLUDE_VULKAN maybe replaced by including vulkan.h, or its alternate,
// which in this case is volk.h in native.hpp
#include "GLFW/glfw3.h"

namespace erhi::vk
{
	struct Window : IWindow {
		Device * mpDevice;
		GLFWwindow * mpWindow;
		VkSurfaceKHR mSurface;
		std::shared_ptr<Queue> mPresentQueue;

		Window(DeviceHandle pDevice, WindowDesc const & desc);
		virtual ~Window() override;
	};
}