#pragma once

#include "../../common/resource/memory.hpp"
#include "../native.hpp"



namespace erhi::dx12 {

	struct Texture : ITexture {
		ComPtr<D3D12MA::Allocation> mpAllocation;
		ComPtr<ID3D12Resource> mpResource;

		Texture(Device * pDevice, MemoryHeapType heapType, TextureDesc const & desc);
		Texture(ComPtr<ID3D12Resource> pResource, TextureDesc const & desc);
		virtual ~Texture() override;
	};

}