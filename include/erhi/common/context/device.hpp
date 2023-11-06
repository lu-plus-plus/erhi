#ifndef ERHI_DEVICE_HPP
#define ERHI_DEVICE_HPP

#include "../common.hpp"



namespace erhi {

	struct IDevice : IObject {
		IDevice();
		virtual ~IDevice() = 0;

		void Log(MessageType type, MessageSeverity severity, std::string_view message);
		void LogVerbose(std::string_view message);
		void LogInfo(std::string_view message);
		void LogWarning(std::string_view message);
		void LogError(std::string_view message);

		virtual IPhysicalDeviceHandle		GetPhysicalDevice() const = 0;

		virtual IQueueHandle				SelectQueue(QueueType queueType) = 0;

		virtual IMemoryHandle				AllocateMemory(MemoryRequirements const & requirements) = 0;

		virtual IBufferHandle				CreateBuffer(MemoryHeapType heapType, BufferDesc const & bufferDesc) = 0;
		virtual MemoryRequirements			GetBufferMemoryRequirements(MemoryHeapType heapType, BufferDesc const & bufferDesc) = 0;
		virtual IBufferHandle				CreatePlacedBuffer(IMemoryHandle memory, uint64_t offset, BufferDesc const & bufferDesc) = 0;

		virtual ITextureHandle				CreateTexture(MemoryHeapType heapType, TextureDesc const & textureDesc) = 0;
		virtual MemoryRequirements			GetTextureMemoryRequirements(MemoryHeapType heapType, TextureDesc const & textureDesc) = 0;
		virtual ITextureHandle				CreatePlacedTexture(IMemoryHandle memory, uint64_t offset, TextureDesc const & textureDesc) = 0;

		//virtual IDescriptorHeap				CreateDescriptorHeap() = 0;
		//virtual IBufferShaderResourceView	CreateShaderResourceView(IBufferHandle buffer, BufferShaderResourceViewDesc const & desc) = 0;
	};

}



#endif // ERHI_DEVICE_HPP