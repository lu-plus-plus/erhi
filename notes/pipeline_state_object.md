The heaviest part in a PSO is pipeline layout (in Vulkan) or root signature (in D3D12).
The complexity of shader module is external - its customization, reflection, compilation, etc. are executed outside the creation of PSOs.

Maybe it's a good idea to create a compute pipeline at first.

