#include "erhi/common/command/dxc_shader.hpp"

#include "erhi/vulkan/context/context.hpp"
#include "erhi/vulkan/command/command.hpp"

namespace erhi::vk {

	ShaderCompiler::ShaderCompiler() : pDxcCompiler(new DxcShaderCompiler()) {}
	ShaderCompiler::~ShaderCompiler() {
		delete pDxcCompiler;
		pDxcCompiler = nullptr;
	}

	IShaderBlobHandle ShaderCompiler::Compile(ShaderCompileInfo const & info) {
		static wchar_t const * extraArguments[] = {
			L"-spirv",
			L"-fspv-target-env=vulkan1.3"
		};
		constexpr size_t extraArgumentCount = sizeof(extraArguments) / sizeof(extraArguments[0]);
		return static_cast<DxcShaderCompiler *>(pDxcCompiler)->compile(info, extraArgumentCount, extraArguments);
	}

	IShaderCompilerHandle Device::CreateShaderCompiler(ShaderCompilerDesc const & desc) {
		return new ShaderCompiler();
	}

}