#include "erhi/dx12/command/command.hpp"

namespace erhi::dx12 {

	ShaderCompiler::ShaderCompiler() = default;
	ShaderCompiler::~ShaderCompiler() = default;

	IShaderBlobHandle ShaderCompiler::compile(ShaderCompileInfo const & info) {
		auto GetTarget = [] (ShaderType shaderType) -> wchar_t const * {
			if (shaderType == ShaderType::Vertex) {
				return L"vs_6_7";
			} else if (shaderType == ShaderType::Pixel) {
				return L"ps_6_7";
			} else if (shaderType == ShaderType::Compute) {
				return L"cs_6_7";
			} else {
				return nullptr;
			}
		};

		wchar_t const * const extraArguments[] = {
			L"-T", GetTarget(info.shaderType)
		};
		constexpr size_t extraArgumentCount = sizeof(extraArguments) / sizeof(extraArguments[0]);
		
		return DxcShaderCompiler::compile(info, extraArgumentCount, extraArguments);
	}

}