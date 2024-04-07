#pragma once

#include "shader.hpp"

#include "wrl/client.h"		// ComPtr
#include "dxcapi.h"			// DirectXCompiler

namespace erhi {
	
	struct DxcShaderCompiler {
		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

		using IDxcCompilerLatest = IDxcCompiler3;

		ComPtr<IDxcUtils> mpUtils;
		ComPtr<IDxcCompilerLatest> mpCompiler;

		DxcShaderCompiler();
		~DxcShaderCompiler();

		IShaderBlobHandle compile(ShaderCompileInfo const & info, uint32_t extraArugmentCount, wchar_t const * const * extraArguments);
	};

	struct DxcShaderBlob : IShaderBlob {
		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

		ComPtr<IDxcBlobEncoding> mpBlobEncoding;
		ComPtr<IDxcResult> mpCompileResult;
		ComPtr<IDxcBlob> mpObject;

		DxcShaderBlob(DxcShaderCompiler & compiler, ShaderCompileInfo const & info, uint32_t extraArugmentCount, wchar_t const * const * extraArguments);

		virtual void const * data() override;
		virtual size_t sizeInBytes() override;
	};

}