#pragma once

#include "../../common/command/command_buffer.hpp"

#include "../native.hpp"

#include <vector>
#include <optional>



namespace erhi::dx12 {

	struct TypedCommandPool;

	struct CommandList : ICommandList {
		ComPtr<ID3D12GraphicsCommandListLatest> mpCommandList;

		TypedCommandPool * mpPool;
		uint32_t mIndexInPool;
		
		CommandList(CommandListDesc const & desc, TypedCommandPool * pPool, uint32_t indexInPool, ID3D12GraphicsCommandListLatest * pCommandList);
		virtual ~CommandList() override;

		virtual void BeginCommands(CommandListBeginInfo const & beginInfo) override;
		virtual void EndCommands() override;

		virtual void SetPrimitiveTopology(PrimitiveTopology pt) override;
		virtual void BindVertexBuffers(uint32_t firstBinding, uint32_t bindingCount, VertexBufferView const * views) override;
		virtual void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;

		virtual void CopyBuffer(IBuffer * dst, uint64_t dstOffset, IBuffer * src, uint64_t srcOffset, uint64_t numBytes) override;

		virtual void CopyDescriptors(
			uint32_t sizeInBytes,
			ICPUDescriptorHeapHandle dstHeap, uint64_t dstOffsetInBytes,
			ICPUDescriptorHeapHandle srcHeap, uint64_t srcOffsetInBytes,
			DescriptorHeapType descriptorHeapsType
		) override;

		virtual void CopyDescriptors(
			uint32_t sizeInBytes,
			IGPUDescriptorHeapHandle dstHeap, uint64_t dstOffsetInBytes,
			ICPUDescriptorHeapHandle srcHeap, uint64_t srcOffsetInBytes,
			DescriptorHeapType descriptorHeapsType
		) override;
	};

	struct TypedCommandPool {
		Device * mpDevice;

		D3D12_COMMAND_LIST_TYPE mCommandListType;
		ComPtr<ID3D12CommandAllocator> mpCommandAllocator;
		std::vector<ComPtr<ID3D12GraphicsCommandList>> mAllLists;
		
		TypedCommandPool(Device * pDevice, D3D12_COMMAND_LIST_TYPE commandListType);
		~TypedCommandPool();

		CommandList * AllocateCommandList(CommandListDesc const & desc);

		void Reset();
	};

	struct CommandPool : ICommandPool {
		Device * mpDevice;

		std::optional<TypedCommandPool> mDirectPool;
		std::optional<TypedCommandPool> mBundlePool;

		CommandPool(Device * pDevice, CommandPoolDesc const & desc);
		virtual ~CommandPool() override;

		virtual void Reset() override;

		virtual ICommandListHandle AllocateCommandList(CommandListDesc const & desc) override;
	};

}