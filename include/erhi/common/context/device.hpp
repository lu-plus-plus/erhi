#ifndef ERHI_DEVICE_HPP
#define ERHI_DEVICE_HPP

#include "../common.hpp"



namespace erhi {

	struct IDevice : IObject {
		IDevice();
		virtual ~IDevice();

		void Info(std::string_view message);
		void Warning(std::string_view message);
		void Error(std::string_view message);

		virtual IPhysicalDeviceHandle		GetPhysicalDevice() const = 0;

		virtual IQueueHandle				SelectQueue(QueueType queueType) = 0;

	private:
		virtual IMemoryHandle				AllocateMemory(MemoryDesc const & desc) = 0;

	private:
		virtual MemoryRequirements			GetBufferMemoryRequirements(MemoryHeapType heapType, BufferDesc const & bufferDesc) = 0;
		virtual IBufferHandle				CreatePlacedBuffer(IMemory * pMemory, uint64_t offset, uint64_t actualSize, BufferDesc const & bufferDesc) = 0;
	public:
		virtual IBufferHandle				CreateCommittedBuffer(MemoryHeapType heapType, BufferDesc const & bufferDesc) = 0;

	private:
		virtual MemoryRequirements			GetTextureMemoryRequirements(MemoryHeapType heapType, TextureDesc const & textureDesc) = 0;
		virtual ITextureHandle				CreatePlacedTexture(IMemory * pMemory, uint64_t offset, uint64_t actualSize, TextureDesc const & textureDesc) = 0;
	public:
		virtual ITextureHandle				CreateCommittedTexture(MemoryHeapType heapType, TextureDesc const & textureDesc) = 0;

	};

}



#endif // ERHI_DEVICE_HPP