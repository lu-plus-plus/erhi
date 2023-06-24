#ifndef ERHI_DEVICE_HPP
#define ERHI_DEVICE_HPP

#include "../common.hpp"



namespace erhi {

	struct IDevice : IObject {
		IDevice();
		virtual ~IDevice();

		void Log(MessageType type, MessageSeverity severity, std::string_view message);
		void Verbose(std::string_view message);
		void Info(std::string_view message);
		void Warning(std::string_view message);
		void Error(std::string_view message);

		virtual IPhysicalDeviceHandle		GetPhysicalDevice() const = 0;

		virtual IQueueHandle				SelectQueue(QueueType queueType) = 0;

		virtual IMemoryHandle				AllocateMemory(MemoryDesc const & desc) = 0;

		virtual MemoryRequirements			GetBufferMemoryRequirements(MemoryHeapType heapType, BufferDesc const & bufferDesc) = 0;
		virtual IBufferHandle				CreateCommittedBuffer(MemoryHeapType heapType, BufferDesc const & bufferDesc) = 0;

		virtual MemoryRequirements			GetTextureMemoryRequirements(MemoryHeapType heapType, TextureDesc const & textureDesc) = 0;
		virtual ITextureHandle				CreateCommittedTexture(MemoryHeapType heapType, TextureDesc const & textureDesc) = 0;

	};

}



#endif // ERHI_DEVICE_HPP