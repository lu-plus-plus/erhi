#include "erhi/vulkan/context/message.hpp"



namespace erhi::vk {

	static MessageSeverity MapMessageSeverity(VkDebugUtilsMessageSeverityFlagBitsEXT native) {
		if (native & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)		return MessageSeverity::Error;
		if (native & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)	return MessageSeverity::Warning;
		if (native & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)		return MessageSeverity::Info;
		return MessageSeverity::Verbose;
	}

	static MessageType MapMessageType(VkDebugUtilsMessageTypeFlagsEXT native) {
		if (native & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)		return MessageType::General;
		if (native & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)	return MessageType::Debug;
		return MessageType::Performance;
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL adaptToMessageCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData,
		void * pUserData) {

		IMessageCallback const * pCallback{ static_cast<IMessageCallback const *>(pUserData) };

		(*pCallback)(MapMessageType(messageType), MapMessageSeverity(messageSeverity), pCallbackData->pMessage);

		return VK_FALSE;
	}

}