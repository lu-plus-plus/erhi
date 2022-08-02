#ifndef ERHI_VULKAN_MESSAGE_HPP
#define ERHI_VULKAN_MESSAGE_HPP

#include "../common/message.hpp"
#include "native.hpp"



namespace erhi::vk {

	VKAPI_ATTR VkBool32 VKAPI_CALL adaptToMessageCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData,
		void * pUserData
	);

}



#endif // ERHI_VULKAN_MESSAGE_HPP