#pragma once

#include "exception.hpp"

namespace erhi::windows {
	char const * ErrorCode(HRESULT result);

	void ThrowOnError(HRESULT result, char const * statement, std::source_location const location = std::source_location::current());

	void ExitOnError(HRESULT result, char const * statement, std::source_location const location = std::source_location::current()) noexcept;
}

#define WindowsThrowOnError(stat) if (HRESULT result = (stat); result != S_OK) erhi::windows::ThrowOnError(result, #stat, std::source_location::current())

#define WindowsExitOnError(stat) if (HRESULT result = (stat); result != S_OK) erhi::windows::ExitOnError(result, #stat, std::source_location::current())
