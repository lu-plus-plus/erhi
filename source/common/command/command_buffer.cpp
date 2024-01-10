#include "erhi/common/command/command_buffer.hpp"

namespace erhi {

	ICommandPool::ICommandPool(CommandPoolDesc const & desc) : mDesc(desc) {}
	ICommandPool::~ICommandPool() = default;

	ICommandList::ICommandList(CommandListDesc const & desc) : mDesc(desc) {}
	ICommandList::~ICommandList() = default;

}