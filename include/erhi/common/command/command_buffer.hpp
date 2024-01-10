#pragma once

#include "../common.hpp"



namespace erhi {

	struct ICommandPool {
		CommandPoolDesc mDesc;

		ICommandPool(CommandPoolDesc const & desc);
		virtual ~ICommandPool() = 0;

		virtual ICommandListHandle AllocateCommandList(CommandListDesc const & desc) = 0;
		virtual void Reset() = 0;
	};

	struct ICommandList {
		CommandListDesc mDesc;

		ICommandList(CommandListDesc const & desc);
		virtual ~ICommandList() = 0;

		virtual void BeginCommands(CommandListBeginInfo const & beginInfo) = 0;
		virtual void EndCommands() = 0;

		//virtual void SetIndexBuffer() = 0;
		//virtual void SetVertexBuffers() = 0;
		//virtual void SetPrimitiveTopology() = 0;

		//virtual void DrawIndexedInstanced() = 0;

		virtual void CopyBuffer(IBuffer * dst, uint64_t dstOffset, IBuffer * src, uint64_t srcOffset, uint64_t numBytes) = 0;
	};

}