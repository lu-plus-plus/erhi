#ifndef ERHI_VULKAN_NATIVE_HPP
#define ERHI_VULKAN_NATIVE_HPP

#include <string>			// for error message in exceptions
#include <exception>		// for base class std::exception

#include "volk.h"



namespace erhi::vk {

	char const * ErrorCode(VkResult result);

	struct bad_api_call : std::exception {

		static constexpr unsigned gInfoSize{ 1024u };
		char mInfo[gInfoSize];

		bad_api_call(VkResult result, char const * literal, unsigned line, char const * file);
		virtual ~bad_api_call() override;

		virtual char const * what() const override;

	};

	struct invalid_arguments : std::exception {
		invalid_arguments(char const * message);
		invalid_arguments(std::string const & message);
		virtual ~invalid_arguments() override;

		virtual char const * what() const override;
	};

	template <typename Insertee, typename Inserted>
	void LinkNext(Insertee & insertee, Inserted & inserted) {
		auto const pNext = insertee.pNext;
		insertee.pNext = &inserted;
		inserted.pNext = pNext;
	}

}

#define vkErrorCode(result) erhi::vk::ErrorCode(result)

#define vkCheckResult(result) if (VkResult r = (result); r != VK_SUCCESS) throw bad_api_call(r, #result, __LINE__, __FILE__)



#endif // ERHI_VULKAN_NATIVE_HPP