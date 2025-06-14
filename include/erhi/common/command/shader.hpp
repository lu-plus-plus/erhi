#pragma once

#include "../common.hpp"
#include "../context/message.hpp"

namespace erhi {

	struct IShaderBlob {
		IShaderBlob();
		virtual ~IShaderBlob() = 0;

		virtual void const * data() = 0;
		virtual size_t sizeInBytes() = 0;
	};

	struct IShaderCompiler {
		IShaderCompiler();
		virtual ~IShaderCompiler() = 0;

		virtual IShaderBlobHandle Compile(ShaderCompileInfo const & info) = 0;
	};

}