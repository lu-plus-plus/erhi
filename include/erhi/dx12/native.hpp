#ifndef ERHI_DX12_NATIVE_HPP
#define ERHI_DX12_NATIVE_HPP

#include <exception>

#include <d3d12.h>
#include <dxgi1_6.h>

#include <comdef.h>
// #include <wrl/client.h>



namespace erhi::dx12 {

	struct bad_api_call : std::exception {
		
		_com_error mComError;

		static constexpr unsigned gInfoSize{ 1024u };
		char mInfo[gInfoSize];

		bad_api_call(HRESULT result, char const * literal, unsigned line, char const * file);
		virtual ~bad_api_call();

		virtual char const * what() const override;

	};

}

#define D3D12CheckResult(stat) if (HRESULT result = (stat); result != S_OK) throw erhi::dx12::bad_api_call(result, #stat, __LINE__, __FILE__)



#endif // !ERHI_DX12_NATIVE_HPP