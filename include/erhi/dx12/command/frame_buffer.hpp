#pragma once

#include "erhi/common/command/frame_buffer.hpp"
#include "../native.hpp"

namespace erhi::dx12 {
	struct FrameBuffer : IFrameBuffer {
		DeviceHandle mpDevice;
		ComPtr<ID3D12DescriptorHeap> mRenderTargetViews;
		ComPtr<ID3D12DescriptorHeap> mDepthStencilViews;

		FrameBuffer(DeviceHandle pDevice, FrameBufferDesc const & desc);
		virtual ~FrameBuffer() override;
	};
}