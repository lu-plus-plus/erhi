In Vulkan, a buffer descriptor does not need to specify a type - all the matters is the type used in shader, no matter it's bulit-in or "structured".

In DX12, a SRV to buffer either specifies a format if it is interpreted as a native type in shader, or has non-zero stride if it's a structured buffer.

The descriptor heaps for RTV and DSV are some "wrapper" for descriptors on CPU side, in essence.

> After recording into the command list (with a call such as OMSetRenderTargets, for example) the memory used to hold the descriptors for this call is immediately available for re-use after the call.
> 
> https://learn.microsoft.com/en-us/windows/win32/direct3d12/non-shader-visible-descriptor-heaps

In a draw call:
- For DX12, all descriptors needed in this draw call must be aggregated in two heaps - one for CBV_SRV_UAV, and the other for samplers.
- For Vulkan, all descriptors have to be in a group of heaps (descriptor buffers) whose number is no more than `maxDescriptorBufferBindings`.
    - No more than `maxSamplerDescriptorBufferBindings` descriptor buffers containing sampler descriptors can be bound.
    - No more than `maxResourceDescriptorBufferBindings` descriptor buffers containing resource descriptors can be bound.
    - If a buffer contains both usage flags, it counts once against both limits.

How do we manage descriptor heaps:
- Provide a max binding count for each backend.
- When the maximum count is not exceeded, and current heaps have been filled, simply create a new heap when more descriptors are needed.
- When the maximum count has been reached, create a new heap which is (significantly) larger than the smallest heap available.
    - Copy its content to the new heap and append new descriptors,
    - switch bindings in the next draw call,
    - and the old, small heap may be released when the previous draw calls are finished and we can make sure it is no longer needed.

How do we create a descriptor buffer (which is really a usual buffer) in Vulkan?
- committed resource: not so good - the number of descriptor buffers can be large.
- placed resource: descriptor allocation is coupled with memory allocator
- Maybe a good choice is to *bind* a memory allcoator to a logical device when the device is created.
    > That means all allcoators must be *virtual* - they define how a range (of memory) is initialized, divided, and released, yet decoupled with the actual memory allocation in backends.

For memory, maybe a native, opaque API handle with metadata (like total size), an offset, and an available size, should be provided.
How do we deal with the "divergent - unified - divergent - unified" problem, which leads to multiple indirections?

> dx::memory : imemory;
> vk::memory : imemory;
> 
> imemory;
>
> linear_allocated_memory : imemory { imemory actual_memory; };
>
> imemory

The actual problem at here is, to retrieve the native memory, two levels of indirections must be dereferenced.
Creating a linear allocation description on heap is not a problem.

> template <alloc> dx::memory : imemory { alloc; }
> template <alloc> vk::memory : imemory { alloc; }
> 
> imemory
>

Q: Is raw device memory from graphics API needed? VMA said that it's not commended, and usually not needed. But how do we allocated memory in multiple memory aliasing, say, frame graph?

A: In section `memory aliasing`, `vmaAllocateMemory` is used, though convenient functions like aliasing buffer creation exist.

## Texture Descriptor

In D3D12, except RTV and DSV which are treated separately, texture descriptors include SRV and UAV. 

In Vulkan, there is no single "view" for a texture. An image view must be created on an image first, and then a descriptor is created with both image view and image layout as arguments.

The key difference at here is the image layout required only in Vulkan.
However, for a usual texture used in shader, there are only several layouts that make sense: VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL.
The difference between these layout roughly matches that of SRV and UAV.

There is a separation between image layout and image view in Vulkan, yet they are fused together as SRV or UAV in D3D12.
It seems that, it can be more efficient to cache VkImageViews for each VkImage, and reuse them whenever possible.
But it needs a hash table, at least a vector, to store the cached VkImageViews - is it worth the cost?

It's a little vague whether a resource' dimension may be reinterpreted or not - in SRV it cannot, but in UAV it's unclear.
Nevertheless, We're not going to support it.

So actually, what's left is only aspect (which is excluded as it's more like the difference between RTV/DSV and other views),
format, and mipmap levels - starting level and level count.

A 64-bit integer is already sufficient for that, though it does not make real difference if it's a little longer.