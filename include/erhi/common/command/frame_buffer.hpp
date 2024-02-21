#pragma once

#include "../common.hpp"

namespace erhi {

	struct IFrameBuffer {
		FrameBufferDesc mDesc;

		IFrameBuffer(FrameBufferDesc const & desc);
		virtual ~IFrameBuffer() = 0;
	};

}