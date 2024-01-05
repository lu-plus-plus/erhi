#include "erhi/common/exception.hpp"

#include <format>



namespace erhi {

	bad_graphics_api_call::bad_graphics_api_call(char const * result, char const * statement, std::source_location const & location) :
		mMessage(std::format(
			"Unexpected result '{}' is returned from graphics API call '{}',\n"
			"\tat line {}, column {},\n"
			"\tin function '{}',\n"
			"\tin file '{}'.\n",
			result, statement,
			location.line(), location.column(),
			location.function_name(),
			location.file_name()
		)) {}

	bad_graphics_api_call::~bad_graphics_api_call() = default;

	char const * bad_graphics_api_call::what() const {
		return mMessage.c_str();
	}

}