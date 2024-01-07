#pragma once

#include "../common.hpp"



namespace erhi {

	struct IDevice {
		DeviceDesc mDesc;
		std::shared_ptr<IMessageCallback> mpMessageCallback;

		IDevice(DeviceDesc const & desc, std::shared_ptr<IMessageCallback> pMessageCallback);
		virtual ~IDevice() = 0;

		virtual IMessageCallback &			Log() const;

		virtual IQueueHandle				SelectQueue(QueueType queueType) = 0;

		virtual IBufferHandle				CreateBuffer(MemoryHeapType heapType, BufferDesc const & bufferDesc) = 0;
		virtual ITextureHandle				CreateTexture(MemoryHeapType heapType, TextureDesc const & textureDesc) = 0;
	};

	namespace vk {
		IDeviceHandle CreateDevice(DeviceDesc const & desc, std::shared_ptr<IMessageCallback> pMessageCallback);
	}

	namespace dx12 {
		IDeviceHandle CreateDevice(DeviceDesc const & desc, std::shared_ptr<IMessageCallback> pMessageCallback);
	}

}