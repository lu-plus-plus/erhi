#pragma once

#include "../common.hpp"



namespace erhi {

	struct IBuffer {
		BufferDesc mDesc;

		IBuffer(BufferDesc const & desc);
		virtual ~IBuffer() = 0;
	};

	struct ITexture {
		TextureDesc mDesc;

		ITexture(TextureDesc const & desc);
		virtual ~ITexture() = 0;
	};

}


