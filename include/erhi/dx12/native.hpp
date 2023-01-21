#ifndef ERHI_DX12_NATIVE_HPP
#define ERHI_DX12_NATIVE_HPP

#include <exception>
#include <source_location>

#include <d3d12.h>
#include <dxgi1_6.h>



namespace erhi::dx12 {

	struct bad_api_call : std::exception {

		static constexpr unsigned gInfoSize{ 1024u };

		char mInfo[gInfoSize];

		bad_api_call(HRESULT result, char const * statement, std::source_location const & location);
		virtual ~bad_api_call();

		virtual char const * what() const override;

	};

	namespace details {

		void ThrowOnError(HRESULT result, char const * statement, std::source_location const location = std::source_location::current());
		
		void ExitOnError(HRESULT result, char const * statement, std::source_location const location = std::source_location::current()) noexcept;

	}

}

#define D3D12CheckResult(stat) if (HRESULT result = (stat); result != S_OK) erhi::dx12::details::ThrowOnError(result, #stat, std::source_location::current())

#define D3D12ExitOnError(stat) if (HRESULT result = (stat); result != S_OK) erhi::dx12::details::ExitOnError(result, #stat, std::source_location::current())



#endif // !ERHI_DX12_NATIVE_HPP