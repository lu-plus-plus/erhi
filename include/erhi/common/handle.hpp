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

	DeclareHandle(Memory);
	DeclareHandle(Buffer);
	DeclareHandle(Texture);
	DeclareHandle(Allocator);

	DeclareHandle(DescriptorHeap);
	DeclareHandle(BufferShaderResourceView);

	#undef DeclareHandle

}