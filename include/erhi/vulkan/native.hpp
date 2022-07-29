#ifndef ERHI_VULKAN_NATIVE_HPP
#define ERHI_VULKAN_NATIVE_HPP

#include <exception>

#include "volk.h"



namespace erhi::vk {

	char const * ErrorCode(VkResult result);

	struct bad_api_call : std::exception {

	private:

		static constexpr unsigned gInfoSize{ 1024u };
		char mInfo[gInfoSize];

	public:

		bad_api_call(VkResult result, char const * literal, unsigned line, char const * file);
		~bad_api_call();

		virtual char const * what() const override;

	};

}

#define vkErrorCode(result) erhi::vk::ErrorCode(result)

#define vkCheckResult(result) if (VkResult r = (result); r != VK_SUCCESS) throw bad_api_call(r, #result, __LINE__, __FILE__)



#endif // ERHI_VULKAN_NATIVE_HPP