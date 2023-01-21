#include <cstdio>

#include <comdef.h>

#include "erhi/dx12/native.hpp"



namespace erhi::dx12 {

	bad_api_call::bad_api_call(HRESULT result, char const * statement, std::source_location const & location) : mInfo() {
		_com_error comError(result);
		std::snprintf(
			mInfo, gInfoSize,
			"Unexpected HRESULT '%s' is returned from DX12 API call '%s',\n"
			"\tat line %u, column %u,\n"
			"\tin function '%s',\n"
			"\tin file '%s'.\n",
			comError.ErrorMessage(), statement, unsigned(location.line()), unsigned(location.column()), location.function_name(), location.file_name()
		);
	}

	bad_api_call::~bad_api_call() = default;

	char const * bad_api_call::what() const {
		return mInfo;
	}



	namespace details {

		void ThrowOnError(HRESULT result, char const * statement, std::source_location const location) {
			throw bad_api_call(result, statement, location);
		}

		void ExitOnError(HRESULT result, char const * statement, std::source_location const location) noexcept {
			bad_api_call e(result, statement, location);
			std::printf("%s", e.what());
			std::terminate();
		}

	}

}