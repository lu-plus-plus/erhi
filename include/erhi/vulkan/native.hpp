#ifndef ERHI_VULKAN_NATIVE_HPP
#define ERHI_VULKAN_NATIVE_HPP

#include <string>			// for error message in exceptions

#include "volk.h"

#include "../common/exception.hpp"



namespace erhi::vk {

	char const * vkErrorCode(VkResult result);

	template <typename T>
	concept HasConstNext = requires (T t) {
		{ t.pNext } -> std::same_as<void const * &>;
	};

	template <typename T>
	concept HasNext = requires (T t) {
		{ t.pNext } -> std::same_as<void * &>;
	};

	struct NextChain {
		void * * ppNext;

		template <HasConstNext T>
		NextChain(T & t) : ppNext(const_cast<void * *>(&t.pNext)) {}

		template <HasNext T>
		NextChain(T & t) : ppNext(&t.pNext) {}

		template <HasConstNext T>
		NextChain & Next(T & t) {
			t.pNext = *ppNext;
			*ppNext = &t;
			ppNext = const_cast<void * *>(&t.pNext);
			return *this;
		}

		template <HasNext T>
		NextChain & Next(T & t) {
			t.pNext = *ppNext;
			*ppNext = &t;
			ppNext = &t.pNext;
			return *this;
		}
	};

}

#define vkCheckResult(result) if (VkResult r = (result); r != VK_SUCCESS) throw bad_api_call(vkErrorCode(r), #result, std::source_location::current())



#endif // ERHI_VULKAN_NATIVE_HPP