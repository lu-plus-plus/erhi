#include <vector>

#include <wrl/client.h>					// ComPtr
#include "dxcapi.h"						// DirectXCompiler

#include "erhi/common/h_result.hpp"		// handling HRESULT
#include "erhi/common/command/shader.hpp"



namespace erhi {

	using namespace windows;

	using Microsoft::WRL::ComPtr;

	void test(void * pSource, uint32_t sourceSizeInBytes, wchar_t const * entryPoint, wchar_t const * target, bool enableDebug, IMessageCallback * pMessageCallback) {
		ComPtr<IDxcUtils> pUtils;
		WindowsThrowOnError(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(pUtils.GetAddressOf())));

		ComPtr<IDxcBlobEncoding> pBlobEncoding;
		WindowsThrowOnError(pUtils->CreateBlob(pSource, sourceSizeInBytes, CP_UTF8, pBlobEncoding.GetAddressOf()));

		std::vector<LPCWSTR> arguments{
			L"-E",
			entryPoint,
			L"-T",
			target,
			DXC_ARG_WARNINGS_ARE_ERRORS,
			enableDebug ? DXC_ARG_DEBUG : DXC_ARG_OPTIMIZATION_LEVEL3
		};

		DxcBuffer const sourceBuffer{
			.Ptr = pBlobEncoding->GetBufferPointer(),
			.Size = pBlobEncoding->GetBufferSize(),
			.Encoding = 0
		};

		ComPtr<IDxcCompiler3> pCompiler;
		WindowsThrowOnError(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(pCompiler.GetAddressOf())));

		ComPtr<IDxcResult> pCompileResult;
		WindowsThrowOnError(pCompiler->Compile(&sourceBuffer, arguments.data(), arguments.size(), nullptr, IID_PPV_ARGS(pCompileResult.GetAddressOf())));

		ComPtr<IDxcBlobUtf8> pErrors;
		WindowsThrowOnError(pCompileResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(pErrors.GetAddressOf()), nullptr));
		if (pErrors && pErrors->GetStringLength() > 0) {
			pMessageCallback->Error(static_cast<char const *>(pErrors->GetBufferPointer()));
			throw std::runtime_error("error in shader compilation");
		}
	}

}
