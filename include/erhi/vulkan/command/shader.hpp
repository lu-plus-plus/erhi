#pragma once

#include "../../common/command/shader.hpp"
#include "../native.hpp"

namespace erhi::vk {

	struct ShaderCompiler : IShaderCompiler {
		ShaderCompiler();
		virtual ~ShaderCompiler() override;

		// type erasure for DXC to avoid its conflicts with GLFW
		void * pDxcCompiler;

		virtual IShaderBlobHandle Compile(ShaderCompileInfo const & info) override;
	};

}