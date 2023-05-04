#include "erhi/common/exception.hpp"

#include <format>



namespace erhi {

	base_exception::base_exception() = default;

	base_exception::~base_exception() = default;

	char const * base_exception::what() const {
		return "erhi base exception";
	}



	bad_api_call::bad_api_call(char const * result, char const * statement, std::source_location const & location) :
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

	bad_api_call::~bad_api_call() = default;

	char const * bad_api_call::what() const {
		return mMessage.c_str();
	}



	invalid_arguments::invalid_arguments(char const * message) : mMessage(message) {}
	invalid_arguments::invalid_arguments(std::string const & message) : mMessage(message) {}
	invalid_arguments::invalid_arguments(std::string && message) : mMessage(std::move(message)) {}

	invalid_arguments::~invalid_arguments() = default;

	char const * invalid_arguments::what() const { return mMessage.c_str(); }

}