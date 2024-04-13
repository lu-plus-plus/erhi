#include <vector>
#include <cassert>

#include "erhi/common/h_result.hpp"			// handling HRESULT
#include "erhi/common/command/shader.hpp"

#include "erhi/common/command/dxc_shader.hpp"



namespace erhi {

	IShaderBlob::IShaderBlob() = default;
	IShaderBlob::~IShaderBlob() = default;

	IShaderCompiler::IShaderCompiler() = default;
	IShaderCompiler::~IShaderCompiler() = default;



	DxcShaderCompiler::DxcShaderCompiler() {
		WindowsThrowOnError(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(mpUtils.GetAddressOf())));
		WindowsThrowOnError(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(mpCompiler.GetAddressOf())));
	}

	DxcShaderCompiler::~DxcShaderCompiler() = default;

	IShaderBlobHandle DxcShaderCompiler::compile(ShaderCompileInfo const & info, uint32_t extraArugmentCount, wchar_t const * const * extraArguments) {
		return new DxcShaderBlob(*this, info, extraArugmentCount, extraArguments);
	}
	


	template <typename ... WS>
		requires (... and std::same_as<WS, LPCWSTR>)
	std::vector<LPCWSTR> MakeArguments(uint32_t extraArugmentCount, wchar_t const * const * extraArguments, WS ... predefined) {
		std::vector<LPCWSTR> arguments;
		arguments.reserve(extraArugmentCount + sizeof...(predefined));
		(arguments.push_back(predefined), ...);
		for (uint32_t i = 0; i < extraArugmentCount; ++i) {
			arguments.push_back(extraArguments[i]);
		}
		return arguments;
	}

	DxcShaderBlob::DxcShaderBlob(DxcShaderCompiler & compiler, ShaderCompileInfo const & info, uint32_t extraArugmentCount, wchar_t const * const * extraArguments) {
		if (info.sourceSizeInBytes != 0) {
			WindowsThrowOnError(compiler.mpUtils->CreateBlob(info.sourceCode, info.sourceSizeInBytes, CP_ACP, mpBlobEncoding.GetAddressOf()));
		}
		else {
			WindowsThrowOnError(compiler.mpUtils->LoadFile(info.fileName, nullptr, mpBlobEncoding.GetAddressOf()));
		}

		auto GetTarget = [] (ShaderType shaderType) -> wchar_t const * {
			if (shaderType == ShaderType::Vertex) {
				return L"vs_6_7";
			}
			else if (shaderType == ShaderType::Pixel) {
				return L"ps_6_7";
			}
			else if (shaderType == ShaderType::Compute) {
				return L"cs_6_7";
			}
			else {
				return nullptr;
			}
		};

		std::vector<LPCWSTR> arguments = MakeArguments(
			extraArugmentCount, extraArguments,
			
			L"-T", GetTarget(info.shaderType),
			L"-E", info.entryPoint,
			DXC_ARG_WARNINGS_ARE_ERRORS,
			info.enableDebug ? DXC_ARG_DEBUG : DXC_ARG_OPTIMIZATION_LEVEL3
		);

		DxcBuffer const sourceBuffer{
			.Ptr = mpBlobEncoding->GetBufferPointer(),
			.Size = mpBlobEncoding->GetBufferSize(),
			.Encoding = 0
		};

		WindowsThrowOnError(compiler.mpCompiler->Compile(&sourceBuffer, arguments.data(), arguments.size(), nullptr, IID_PPV_ARGS(mpCompileResult.GetAddressOf())));

		// to suppress Visual Studio C6387
		ComPtr<IDxcBlobWide> pDummyName;

		ComPtr<IDxcBlobUtf8> pErrors;
		WindowsThrowOnError(mpCompileResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(pErrors.GetAddressOf()), pDummyName.GetAddressOf()));
		if (pErrors && pErrors->GetStringLength() > 0) {
			info.pMessageCallback->Error(static_cast<char const *>(pErrors->GetBufferPointer()));
			throw std::runtime_error("error in shader compilation");
		}

		WindowsThrowOnError(mpCompileResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(mpObject.GetAddressOf()), pDummyName.GetAddressOf()));
	}

	void const * DxcShaderBlob::data() {
		return mpObject->GetBufferPointer();
	}

	size_t DxcShaderBlob::sizeInBytes() {
		return mpObject->GetBufferSize();
	}

}
