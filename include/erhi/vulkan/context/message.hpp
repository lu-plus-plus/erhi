#pragma once

#include "../../common/context/message.hpp"
#include "../native.hpp"



namespace erhi::vk {

	VKAPI_ATTR VkBool32 VKAPI_CALL adaptToMessageCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData,
		void * pUserData
	);

}
