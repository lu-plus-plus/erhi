#include "erhi/vulkan/context/context.hpp"
#include "erhi/vulkan/present/present.hpp"
#include "erhi/vulkan/command/command.hpp"

#include "magic_enum.hpp"

#include <format>
#include <algorithm>

namespace erhi::vk {

	SwapChain::SwapChain(DeviceHandle pDevice, WindowHandle pWindow, SwapChainDesc const & desc) :
		ISwapChain(desc), mpDevice(pDevice), mpWindow(pWindow), mSwapChain(VK_NULL_HANDLE) {
		
		VkFormat const format = mapping::MapFormat(desc.format);
		VkColorSpaceKHR const colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

		bool const isFormatSupported = std::any_of(
			pWindow->mSurfaceFormats.begin(), pWindow->mSurfaceFormats.end(),
			[format, colorSpace] (VkSurfaceFormatKHR const & f) -> bool { return format == f.format and colorSpace == f.colorSpace; }
		);

		if (not isFormatSupported) {
			throw std::runtime_error(std::format(
				"swap chain with format {} and color space nonlinear-RGB is not supported",
				magic_enum::enum_name(desc.format)
			));
		}

		VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
		for (VkPresentModeKHR m : pWindow->mSurfacePresentModes) if (m == VK_PRESENT_MODE_MAILBOX_KHR) {
			presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
		}

		uint32_t const imageCount = desc.bufferCount;
		if (not (pWindow->mSurfaceCapabilities.minImageCount <= desc.bufferCount and desc.bufferCount <= pWindow->mSurfaceCapabilities.maxImageCount)) {
			throw std::runtime_error(std::format(
				"buffer count in swap chain must be between {} and {}",
				pWindow->mSurfaceCapabilities.minImageCount, pWindow->mSurfaceCapabilities.maxImageCount
			));
		}

		VkSwapchainCreateInfoKHR const createInfo = {
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.pNext = nullptr,
			.flags = 0,
			.surface = pWindow->mSurface,
			.minImageCount = desc.bufferCount,
			.imageFormat = format,
			.imageColorSpace = colorSpace,
			.imageExtent = VkExtent2D{ .width = pWindow->mCurrentWidth, .height = pWindow->mCurrentHeight },
			.imageArrayLayers = 1,
			.imageUsage = mapping::MapTextureUsage(desc.usageFlags),
			.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 1,
			.pQueueFamilyIndices = &pWindow->mPresentQueue->mQueueFamilyIndex,
			.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode = presentMode,
			.clipped = VK_FALSE,
			.oldSwapchain = VK_NULL_HANDLE
		};

		vkCheckResult(vkCreateSwapchainKHR(*mpDevice, &createInfo, nullptr, &mSwapChain));
	}

	SwapChain::~SwapChain() {
		vkDestroySwapchainKHR(*mpDevice, mSwapChain, nullptr);
	}

}