There is a key difference of command list (buffer) between D3D12 and Vulkan:

The command list in DX12 can be reset and reused immendiately after submit, but Vulkan requires that

> "applications must not attempt to modify the command buffer in any way".

https://learn.microsoft.com/en-us/windows/win32/direct3d12/recording-command-lists-and-bundles

https://registry.khronos.org/vulkan/specs/1.2-khr-extensions/html/chap6.html#commandbuffers-lifecycle

So there exists no general "ResetCommandBuffer", as in Vulkan it requires user to synchronize with fences, while in DX12, nothing.

There is one other option: Classify command buffers into two kinds, transient (one-time) ones and persistent ones.
This classification is explicited specified in Vulkan command **pool**, yet does not exist in DX12.

It seems that, at least on NVIDIA devices, resetting one whole command pool is cheaper than individually resetting command buffers.
> GDC 2016, Vulkan: the essentials, https://developer.download.nvidia.com/gameworks/events/GDC2016/Vulkan_Essentials_GDC16_tlorach.pdf

Perhaps the best design is to create one default transient command pool, per thread, per frame.
Threads may allocate one optional persistent command pool for those pre-recordable, "static" tasks.
From the perspective of RHI, the classification between transient pool and persistent pool is enough.

> "Don’t expect lots of list reuse."
> There are usually many per-frame changes in terms of objects visibility etc.
> Post-processing may be an exception.
> 
> https://developer.nvidia.com/dx12-dos-and-donts

> Reusing command list in DX12 can be a application-level optimization. Persistent command lists do not get reset,
> whole transient lists can be reset once it's submitted, though it may not be the best choice.
>
> https://github.com/Microsoft/DirectX-Graphics-Samples/issues/270
>
> From this issue, we know that resetting DX12 command list still makes sense, performance-wise: it avoids increasing memory usage.

There are a lot of valuable discussions on Reddit.
> https://www.reddit.com/r/vulkan/comments/5zwfot/whats_your_command_buffer_allocation_strategy/
>
> https://www.reddit.com/r/vulkan/comments/59c6bu/rebuilding_command_buffer_each_frame/

Vulkan allows primary and secondary command buffers to be allocated in the same pool,
while D3D12 requires a command allocator to be initialized with a is-bundle-or-not enum.

If the command list type is specified as "bundle", D3D12 command allocator does not need to be binded with a queue type,
while in Vulkan a queue family is always required.

That is to say, the ways of management of secondary command buffer, or bundle, are intrinsically different in the two GFX APIs.
In Vulkan, a command buffer simply allocates secondary command buffers in the same pool as itself's,
while in D3D12, it would be better to share bundle allocator among all the direct allocaters of one thread.

The smallest granularity of command buffer **management** should be thread - it is common in all APIs that
lifetime of command buffers is binded with thread.

There are at least three lifetimes for command buffer:
- one-time: It should be the most common one - command buffers get recorded, committed, and dropped.
- short-lived: The only usage that I can come up with now is repeated bundle - for example, the loop body of a algorithm on GPU.
- presistent: Render graphs, etc. They can live up many frames.

## Bundle Inheritance

In DX12, bundles inherit all states in the direct command list, but PSO state, and IA primitive topology.
Any state changed by a bundle also takes effect in the direct command list on which it's executed.

> https://microsoft.github.io/DirectX-Specs/d3d/CPUEfficiency.html#bundle-state-inheritance-and-leakage

In Vulkan, **all** command buffers, not matter primary or secondary, being recorded in the same command buffer or not, have **no** inheritance to each another.

> There is an exception of VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT used together with `VkCommandBufferBeginInfo.pInheritanceInfo`, but it's actually very specific and is not broadly adpoted.

The basic idea: make descriptor sets immutable - i.e., always create new descriptor sets when update is needed, instead of modifying existing ones.

That is because in any meaningful usage case, the descriptor sets must be being used by some shaders, and thus updates must be delayed - creating new descriptor sets simplifies resource management, at the cost of more memory usage.

It's usually beneficial to maintain the states of a command list. When executing a bundle, inheritance needs to be explicitly specified.

1. Whenever the inheritance of a state is not needed, user may simply drop it in specifying inheritance.
2. Whenever the inheritance of a state is needed, 