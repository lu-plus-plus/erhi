#pragma once

#include "../common.hpp"

#include <vector>

namespace erhi {

	struct IFrameBuffer {
		IRenderPassHandle mpRenderPass;
		std::vector<ITextureViewHandle> mAttachments;
		uint32_t mWidth;
		uint32_t mHeight;

		IFrameBuffer(FrameBufferDesc const & desc);
		virtual ~IFrameBuffer() = 0;
	};

}