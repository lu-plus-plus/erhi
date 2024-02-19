#include "erhi/vulkan/context/context.hpp"
#include "erhi/vulkan/command/command.hpp"
#include "erhi/vulkan/present/present.hpp"

#include <format>
#include <algorithm>

#include "magic_enum.hpp"



namespace erhi::vk
{
	Window::Window(DeviceHandle pDevice, WindowDesc const & desc) : IWindow(desc), mpDevice(pDevice), mpWindow(nullptr), mSurface(VK_NULL_HANDLE) {
		mpWindow = glfwCreateWindow(mCurrentWidth, mCurrentHeight, "Vulkan", nullptr, nullptr);
		if (mpWindow == nullptr) {
			throw std::runtime_error("failed to create a new GLFW window");
		}

		vkCheckResult(glfwCreateWindowSurface(pDevice->mInstance, mpWindow, nullptr, &mSurface));

		vkCheckResult(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*mpDevice->mpPhysicalDevice, mSurface, &mSurfaceCapabilities));

		// format and color space support

		uint32_t formatCount;
		vkCheckResult(vkGetPhysicalDeviceSurfaceFormatsKHR(*mpDevice->mpPhysicalDevice, mSurface, &formatCount, nullptr));

		if (formatCount != 0) {
			mSurfaceFormats.resize(formatCount);
			vkCheckResult(vkGetPhysicalDeviceSurfaceFormatsKHR(*mpDevice->mpPhysicalDevice, mSurface, &formatCount, mSurfaceFormats.data()));
		}

		// present mode

		uint32_t presentModeCount;
		vkCheckResult(vkGetPhysicalDeviceSurfacePresentModesKHR(*mpDevice->mpPhysicalDevice, mSurface, &presentModeCount, nullptr));

		if (presentModeCount != 0) {
			mSurfacePresentModes.resize(presentModeCount);
			vkCheckResult(vkGetPhysicalDeviceSurfacePresentModesKHR(*mpDevice->mpPhysicalDevice, mSurface, &presentModeCount, mSurfacePresentModes.data()));
		}
		
		// present queue

		auto GetSurfaceSupport = [&] (uint32_t queueFamilyIndex) -> bool {
			VkBool32 supported = false;
			vkCheckResult(vkGetPhysicalDeviceSurfaceSupportKHR(*pDevice->mpPhysicalDevice, queueFamilyIndex, mSurface, &supported));
			return supported;
		};

		if (GetSurfaceSupport(pDevice->mPrimaryQueue->mQueueFamilyIndex)) {
			mPresentQueue = pDevice->mPrimaryQueue;
		}
		else {
			throw std::runtime_error("VkSurface is not supported on the primary Graphics, Compute & Transfer queue");
		}
	}

	Window::~Window() {
		vkDestroySurfaceKHR(mpDevice->mInstance, mSurface, nullptr);
		glfwDestroyWindow(mpWindow);
	}

	IWindowHandle Device::CreateNewWindow(WindowDesc const & desc) {
		return new Window(this, desc);
	}



	ISwapChainHandle Window::CreateSwapChain(SwapChainDesc const & desc) {
		return new SwapChain(mpDevice, this, desc);
	}

}