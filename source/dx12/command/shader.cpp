#include "erhi/dx12/context/context.hpp"
#include "erhi/dx12/command/command.hpp"

namespace erhi::dx12 {

	ShaderCompiler::ShaderCompiler() = default;
	ShaderCompiler::~ShaderCompiler() = default;

	IShaderBlobHandle ShaderCompiler::compile(ShaderCompileInfo const & info) {
		// <todo>
		// a custom way to specify Vulkan qualifiers
		// </todo>
		wchar_t const * const extraArguments[] = {
			L"-Wno-ignored-attributes"
		};
		constexpr size_t extraArgumentCount = sizeof(extraArguments) / sizeof(extraArguments[0]);
		
		return DxcShaderCompiler::compile(info, extraArgumentCount, extraArguments);
	}

	IShaderCompilerHandle Device::CreateShaderCompiler(ShaderCompilerDesc const & desc) {
		return new ShaderCompiler();
	}

}