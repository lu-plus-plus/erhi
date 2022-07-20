#ifndef ERHI_VULKAN_MESSAGE_HPP
#define ERHI_VULKAN_MESSAGE_HPP

#include <vulkan/vulkan.h>



namespace erhi::vk {

	VKAPI_ATTR VkBool32 VKAPI_CALL ForwardDebugUtilsCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData,
		void * pUserData
	);

}



#endif // ERHI_VULKAN_MESSAGE_HPP