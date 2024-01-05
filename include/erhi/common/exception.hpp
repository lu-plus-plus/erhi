#pragma once

#include <exception>			// exception
#include <stdexcept>			// all kinds of error
#include <string>				// error message
#include <source_location>		// where exception is thrown



namespace erhi {

	struct bad_graphics_api_call : std::exception {
		std::string mMessage;

		bad_graphics_api_call(char const * result, char const * statement, std::source_location const & location);
		virtual ~bad_graphics_api_call() override;

		virtual char const * what() const override;
	};

}