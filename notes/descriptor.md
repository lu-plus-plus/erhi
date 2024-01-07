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