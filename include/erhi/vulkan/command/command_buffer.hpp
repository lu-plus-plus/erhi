#pragma once

#include "../../common/command/command_buffer.hpp"
#include "../native.hpp"

#include <vector>



namespace erhi::vk {

	struct CommandPool : ICommandPool {
		Device * mpDevice;
		VkCommandPool mCommandPool;
		std::vector<VkCommandBuffer> mAllocatedPrimaryBuffers;
		std::vector<VkCommandBuffer> mFreePrimaryBuffers;
		std::vector<VkCommandBuffer> mAllocatedSecondaryBuffers;
		std::vector<VkCommandBuffer> mFreeSecondaryBuffers;

		CommandPool(CommandPoolDesc const & desc, Device * pDevice);
		virtual ~CommandPool() override;
		
		virtual ICommandListHandle AllocateCommandList(CommandListDesc const & desc) override;
		virtual void Reset() override;
	};

	struct CommandList : ICommandList {
		VkCommandBuffer mCommandBuffer;
		
		CommandList(CommandListDesc const & desc, VkCommandBuffer commandBuffer);
		virtual ~CommandList() override;

		virtual void BeginCommands(CommandListBeginInfo const & beginInfo) override;
		virtual void EndCommands() override;

		void CopyBuffer(VkBuffer dst, uint64_t dstOffset, VkBuffer src, uint64_t srcOffset, uint64_t numBytes);
		virtual void CopyBuffer(IBuffer * dst, uint64_t dstOffset, IBuffer * src, uint64_t srcOffset, uint64_t numBytes) override;

		virtual void SetPrimitiveTopology(PrimitiveTopology pt) override;
		virtual void BindVertexBuffers(uint32_t firstBinding, uint32_t bindingCount, VertexBufferView const * views) override;
		virtual void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;

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

}