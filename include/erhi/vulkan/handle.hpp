#ifndef ERHI_VULKAN_HANDLE_HPP
#define ERHI_VULKAN_HANDLE_HPP

#include "../common/handle.hpp"



namespace erhi::vk {

#define DeclareHandle(type) using type ## Handle = Handle<struct type>

	DeclareHandle(Instance);
	DeclareHandle(PhysicalDevice);
	DeclareHandle(Device);
	DeclareHandle(MessageCallback);

	DeclareHandle(Queue);

#undef DeclareHandle

}



#endif // ERHI_VULKAN_HANDLE_HPP