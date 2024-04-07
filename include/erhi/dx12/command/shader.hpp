#pragma once

#include "../../common/command/shader.hpp"
#include "../../common/command/dxc_shader.hpp"
#include "../native.hpp"

namespace erhi::dx12 {

	struct ShaderCompiler : IShaderCompiler, DxcShaderCompiler {
		ShaderCompiler();
		virtual ~ShaderCompiler() override;

		virtual IShaderBlobHandle compile(ShaderCompileInfo const & info) override;
	};

}