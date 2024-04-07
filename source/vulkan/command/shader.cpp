#include "erhi/common/command/dxc_shader.hpp"
#include "erhi/vulkan/command/shader.hpp"

namespace erhi::vk {

	ShaderCompiler::ShaderCompiler() : pDxcCompiler(new DxcShaderCompiler()) {}
	ShaderCompiler::~ShaderCompiler() {
		delete pDxcCompiler;
		pDxcCompiler = nullptr;
	}

	IShaderBlobHandle ShaderCompiler::compile(ShaderCompileInfo const & info) {
		static wchar_t const * extraArguments[] = {
			L"-fspv-target-env=vulkan1.3"
		};
		constexpr size_t extraArgumentCount = sizeof(extraArguments) / sizeof(extraArguments[0]);
		return static_cast<DxcShaderCompiler *>(pDxcCompiler)->compile(info, extraArgumentCount, extraArguments);
	}

}