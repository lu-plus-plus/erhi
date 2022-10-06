#include <cstdio>

#include "erhi/dx12/native.hpp"

namespace erhi::dx12 {

	bad_api_call::bad_api_call(HRESULT result, char const * literal, unsigned line, char const * file) : mComError(result), mInfo{} {
		std::snprintf(
			mInfo, gInfoSize,
			"`%s` is returned from DX12 API call\n"
			"\t`%s`\n"
			"\tat line %u, file %s.\n",
			mComError.ErrorMessage(), literal, line, file
		);
	}

	bad_api_call::~bad_api_call() = default;

	char const * bad_api_call::what() const {
		return mInfo;
	}

}