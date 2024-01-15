#include <comdef.h>

#include <print>

#include "erhi/common/h_result.hpp"



namespace erhi::windows {

	char const * ErrorCode(HRESULT result) {
		_com_error comError(result);
		return comError.ErrorMessage();
	}

	void ThrowOnError(HRESULT result, char const * statement, std::source_location const location) {
		throw bad_graphics_api_call(ErrorCode(result), statement, location);
	}

	void ExitOnError(HRESULT result, char const * statement, std::source_location const location) noexcept {
		bad_graphics_api_call e(ErrorCode(result), statement, location);
		std::print("{}", e.what());
		std::terminate();
	}

}