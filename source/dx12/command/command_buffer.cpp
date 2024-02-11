#include "erhi/dx12/context/context.hpp"
#include "erhi/dx12/resource/resource.hpp"
#include "erhi/dx12/command/command.hpp"

#include <cassert>



namespace erhi::dx12 {

	CommandList::CommandList(
		CommandListDesc const & desc,
		TypedCommandPool * pPool, uint32_t indexInPool,
		ID3D12GraphicsCommandListLatest * pCommandList
	) : ICommandList(desc), mpCommandList(pCommandList), mpPool(pPool), mIndexInPool(indexInPool) {}

	CommandList::~CommandList() {
		mpCommandList.Reset();
	}

	void CommandList::BeginCommands(CommandListBeginInfo const & beginInfo) {}

	void CommandList::EndCommands() {
		D3D12CheckResult(mpCommandList->Close());
	}

	void CommandList::SetPrimitiveTopology(PrimitiveTopology pt) {
		mpCommandList->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(1 + static_cast<int>(pt)));
	}

	void CommandList::BindVertexBuffers(uint32_t firstBinding, uint32_t bindingCount, VertexBufferView const * views) {
		std::vector<D3D12_VERTEX_BUFFER_VIEW> vbvs(bindingCount);
		for (auto i = 0u; i < bindingCount; ++i) {
			vbvs[i].BufferLocation = dynamic_cast<Buffer *>(views[i].pBuffer)->mpResource->GetGPUVirtualAddress() + views[i].offset;
			vbvs[i].SizeInBytes = UINT(views[i].size);
			vbvs[i].StrideInBytes = UINT(views[i].stride);
		}
		mpCommandList->IASetVertexBuffers(firstBinding, bindingCount, vbvs.data());
	}

	void CommandList::DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
		mpCommandList->DrawInstanced(vertexCount, instanceCount, firstVertex, firstInstance);
	}

	void CommandList::CopyBuffer(IBuffer * dst, uint64_t dstOffset, IBuffer * src, uint64_t srcOffset, uint64_t numBytes) {
		mpCommandList->CopyBufferRegion(
			dynamic_cast<Buffer *>(dst)->mpResource.Get(), dstOffset,
			dynamic_cast<Buffer *>(src)->mpResource.Get(), srcOffset,
			numBytes
		);
	}



	void CommandList::CopyDescriptors(
		uint32_t sizeInBytes,
		ICPUDescriptorHeapHandle dstHeap, uint64_t dstOffsetInBytes,
		ICPUDescriptorHeapHandle srcHeap, uint64_t srcOffsetInBytes,
		DescriptorHeapType descriptorHeapsType) {

		auto dst = dynamic_cast<CPUDescriptorHeapHandle>(dstHeap);
		auto src = dynamic_cast<CPUDescriptorHeapHandle>(srcHeap);

		assert(dst->mDesc.type == src->mDesc.type);

		mpPool->mpDevice->mpDevice->CopyDescriptorsSimple(
			sizeInBytes / dst->mDescriptorSizeInBytes,
			dst->CPUDescriptorHandleAt(dstOffsetInBytes),
			src->CPUDescriptorHandleAt(srcOffsetInBytes),
			MapDescriptorHeapType(descriptorHeapsType)
		);
	}

	void CommandList::CopyDescriptors(
		uint32_t sizeInBytes,
		IGPUDescriptorHeapHandle dstHeap, uint64_t dstOffsetInBytes,
		ICPUDescriptorHeapHandle srcHeap, uint64_t srcOffsetInBytes,
		DescriptorHeapType descriptorHeapsType) {

		auto dst = dynamic_cast<CPUDescriptorHeapHandle>(dstHeap);
		auto src = dynamic_cast<GPUDescriptorHeapHandle>(srcHeap);

		assert(dst->mDesc.type == src->mDesc.type);

		mpPool->mpDevice->mpDevice->CopyDescriptorsSimple(
			sizeInBytes / dst->mDescriptorSizeInBytes,
			dst->CPUDescriptorHandleAt(dstOffsetInBytes),
			src->CPUDescriptorHandleAt(srcOffsetInBytes),
			MapDescriptorHeapType(descriptorHeapsType)
		);
	}



	TypedCommandPool::TypedCommandPool(Device * pDevice, D3D12_COMMAND_LIST_TYPE commandListType) : mpDevice(pDevice), mCommandListType(commandListType) {
		D3D12CheckResult(pDevice->mpDevice->CreateCommandAllocator(commandListType, IID_PPV_ARGS(mpCommandAllocator.GetAddressOf())));
	}

	TypedCommandPool::~TypedCommandPool() {
		mpCommandAllocator.Reset();
	}

	CommandList * TypedCommandPool::AllocateCommandList(CommandListDesc const & desc) {
		ComPtr<ID3D12GraphicsCommandListLatest> pCommandList = nullptr;
		D3D12CheckResult(mpDevice->mpDevice->CreateCommandList(0, mCommandListType, mpCommandAllocator.Get(), nullptr, IID_PPV_ARGS(pCommandList.GetAddressOf())));
		uint32_t const indexInPool = mAllLists.size();

		return new CommandList(desc, this, indexInPool, pCommandList.Get());
	}

	void TypedCommandPool::Reset() {
		D3D12CheckResult(mpCommandAllocator->Reset());
	}



	CommandPool::CommandPool(Device * pDevice, CommandPoolDesc const & desc) : ICommandPool(desc), mpDevice(pDevice) {}

	CommandPool::~CommandPool() = default;

	void CommandPool::Reset() {
		if (mDirectPool) mDirectPool.value().Reset();
		if (mBundlePool) mBundlePool.value().Reset();
	}

	ICommandListHandle CommandPool::AllocateCommandList(CommandListDesc const & desc) {
		D3D12_COMMAND_LIST_TYPE listType = D3D12_COMMAND_LIST_TYPE_DIRECT;
		std::optional<TypedCommandPool> * pPool = nullptr;

		if (desc.level == CommandListLevel::Direct) {
			listType = MapQueueType(mDesc.queueType);
			pPool = &mDirectPool;
		}
		else {
			listType = D3D12_COMMAND_LIST_TYPE_BUNDLE;
			pPool = &mBundlePool;
		}

		if (not pPool->has_value()) pPool->emplace(mpDevice, listType);

		return pPool->value().AllocateCommandList(desc);
	}



	ICommandPoolHandle Device::CreateCommandPool(CommandPoolDesc const & desc) {
		return new CommandPool(this, desc);
	}

}