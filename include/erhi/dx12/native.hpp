#ifndef ERHI_DX12_NATIVE_HPP
#define ERHI_DX12_NATIVE_HPP

#include <d3d12.h>
#include <dxgi1_6.h>

#include "../common/exception.hpp"



namespace erhi::dx12 {

	char const * ErrorCode(HRESULT result);

	namespace details {

		void ThrowOnError(HRESULT result, char const * statement, std::source_location const location = std::source_location::current());
		
		void ExitOnError(HRESULT result, char const * statement, std::source_location const location = std::source_location::current()) noexcept;

	}

}

#define D3D12CheckResult(stat) if (HRESULT result = (stat); result != S_OK) erhi::dx12::details::ThrowOnError(result, #stat, std::source_location::current())

#define D3D12ExitOnError(stat) if (HRESULT result = (stat); result != S_OK) erhi::dx12::details::ExitOnError(result, #stat, std::source_location::current())



#endif // !ERHI_DX12_NATIVE_HPP