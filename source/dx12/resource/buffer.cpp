#include "erhi/dx12/context/context.hpp"
#include "erhi/dx12/resource/resource.hpp"



namespace erhi::dx12 {

	namespace {
		D3D12_RESOURCE_FLAGS MapBufferUsageFlags(BufferUsageFlags bufferUsage) {
			D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

			// In D3D12, resource flags are mostly for textures.
			if (bufferUsage & BufferUsageUnorderedAccess) flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

			return flags;
		}

		D3D12_RESOURCE_DESC1 GetD3D12ResourceDesc(BufferDesc const & bufferDesc) {
			/*
				<todo>
				Per D3D12 document,
				"However, applications can use the most amount of capability support without concern about the efficiency impact on buffers."
				</todo>
			*/
			D3D12_RESOURCE_DESC1 const resourceDesc{
				.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
				.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
				.Width = bufferDesc.size,
				.Height = 1,
				.DepthOrArraySize = 1,
				.MipLevels = 1,
				.Format = DXGI_FORMAT_UNKNOWN,
				.SampleDesc = DXGI_SAMPLE_DESC{
					.Count = 1,
					.Quality = 0
				},
				.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
				.Flags = MapBufferUsageFlags(bufferDesc.usage),
				.SamplerFeedbackMipRegion = D3D12_MIP_REGION{ 0, 0, 0 },
			};
			return resourceDesc;
		}
	}



	Buffer::Buffer(Device * pDevice, MemoryHeapType heapType, BufferDesc const & desc) : IBuffer(desc), mpAllocation(nullptr), mpResource(nullptr) {
		D3D12MA::ALLOCATION_DESC allocationDesc = {};
		allocationDesc.HeapType = mapping::MapMemoryHeapType(heapType);

		auto const resourceDesc = GetD3D12ResourceDesc(desc);

		D3D12CheckResult(pDevice->mpMemoryAllocator->CreateResource3(&allocationDesc, &resourceDesc, D3D12_BARRIER_LAYOUT_UNDEFINED, nullptr, 0, nullptr, mpAllocation.GetAddressOf(), IID_PPV_ARGS(mpResource.GetAddressOf())));
	}

	Buffer::~Buffer() {
		mpResource.Reset();
		mpAllocation.Reset();
	}



	IBufferHandle Device::CreateBuffer(MemoryHeapType heapType, BufferDesc const & bufferDesc) {
		return new Buffer(this, heapType, bufferDesc);
	}

}