#include <comdef.h>

#include <print>

#include "erhi/dx12/native.hpp"



namespace erhi::dx12 {

	char const * ErrorCode(HRESULT result) {
		return windows::ErrorCode(result);
	}

	void ThrowOnError(HRESULT result, char const * statement, std::source_location const location) {
		windows::ThrowOnError(result, statement, location);
	}

	void ExitOnError(HRESULT result, char const * statement, std::source_location const location) noexcept {
		windows::ExitOnError(result, statement, location);
	}

}