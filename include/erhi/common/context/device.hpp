#pragma once

#include "../common.hpp"



namespace erhi {

	struct IDevice {
		DeviceDesc mDesc;
		std::shared_ptr<IMessageCallback> mpMessageCallback;

		IDevice(DeviceDesc const & desc);
		virtual ~IDevice() = 0;

		virtual IMessageCallback &			Log() const;
		virtual IPhysicalDeviceHandle		GetPhysicalDevice() const = 0;

		virtual IQueueHandle				SelectQueue(QueueType queueType) = 0;

		virtual IBufferHandle				CreateBuffer(MemoryHeapType heapType, BufferDesc const & bufferDesc) = 0;
		virtual ITextureHandle				CreateTexture(MemoryHeapType heapType, TextureDesc const & textureDesc) = 0;
	};

}