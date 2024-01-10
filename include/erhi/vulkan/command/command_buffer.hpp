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

		virtual void CopyBuffer(IBuffer * dst, uint64_t dstOffset, IBuffer * src, uint64_t srcOffset, uint64_t numBytes) override;
	};

}