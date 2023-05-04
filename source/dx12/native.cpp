#include <cstdio>

#include <comdef.h>

#include "erhi/dx12/native.hpp"



namespace erhi::dx12 {

	char const * ErrorCode(HRESULT result) {
		_com_error comError(result);
		return comError.ErrorMessage();
	}

	namespace details {

		void ThrowOnError(HRESULT result, char const * statement, std::source_location const location) {
			throw bad_api_call(ErrorCode(result), statement, location);
		}

		void ExitOnError(HRESULT result, char const * statement, std::source_location const location) noexcept {
			bad_api_call e(ErrorCode(result), statement, location);
			std::printf("%s", e.what());
			std::terminate();
		}

	}

}