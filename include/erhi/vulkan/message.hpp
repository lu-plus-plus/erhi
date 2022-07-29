#ifndef ERHI_VULKAN_MESSAGE_HPP
#define ERHI_VULKAN_MESSAGE_HPP

#include "erhi/vulkan/native.hpp"
#include "erhi/common/message.hpp"



namespace erhi::vk {

	VKAPI_ATTR VkBool32 VKAPI_CALL adaptToMessageCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData,
		void * pUserData
	);

}



#endif // ERHI_VULKAN_MESSAGE_HPP