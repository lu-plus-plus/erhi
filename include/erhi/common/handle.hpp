#pragma once

#include <memory>



namespace erhi {

	#define DeclareHandle(type) \
		using I ## type ## Handle = struct I ## type *; \
		namespace vk { using type ## Handle = struct type *; } \
		namespace dx12 { using type ## Handle = struct type *; }

	DeclareHandle(MessageCallback);
	DeclareHandle(Instance);
	DeclareHandle(PhysicalDevice);
	DeclareHandle(Device);

	DeclareHandle(Queue);
	DeclareHandle(CommandPool);
	DeclareHandle(CommandList);

	DeclareHandle(Memory);
	DeclareHandle(Buffer);
	DeclareHandle(Texture);
	DeclareHandle(Allocator);

	DeclareHandle(CPUDescriptorHeap);
	DeclareHandle(GPUDescriptorHeap);
	DeclareHandle(DescriptorSet);
	DeclareHandle(DescriptorSetLayout);

	DeclareHandle(BufferShaderResourceView);
	DeclareHandle(BufferUnorderedAccessView);
	DeclareHandle(TextureView);
	DeclareHandle(TextureShaderResourceView);
	DeclareHandle(TextureUnorderedAccessView);

	DeclareHandle(Window);

	#undef DeclareHandle

}