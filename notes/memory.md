There is a parameter "alignment" in creating D3D12 memory heap.

When we are trying to create overlapped 1-sample textures and MSAA textures, the memory requirements returned from MSAA textures aligns to 4MB.
Reducing the maximum alignment between all resources' requirements naturally leads to the correct result, 4MB.

However, there is this small texture.
The spec requires memory heaps align to 4MB when MSAA is used, but small MSAA textures only align to 64KB.