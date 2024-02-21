#pragma once

#include "../../common/command/frame_buffer.hpp"
#include "../native.hpp"

namespace erhi::vk {

	struct FrameBuffer : IFrameBuffer {
		DeviceHandle mpDevice;
		VkFramebuffer mFrameBuffer;

		FrameBuffer(DeviceHandle pDevice, FrameBufferDesc const & desc);
		virtual ~FrameBuffer() override;
	};

}