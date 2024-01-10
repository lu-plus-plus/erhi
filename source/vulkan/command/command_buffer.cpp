#include "erhi/vulkan/command/command.hpp"
#include "erhi/vulkan/context/device.hpp"
#include "erhi/vulkan/resource/memory.hpp"


namespace erhi::vk {

	ICommandPoolHandle Device::CreateCommandPool(CommandPoolDesc const & desc) {
		return new CommandPool(desc, this);
	}



	CommandPool::CommandPool(CommandPoolDesc const & desc, Device * pDevice) : ICommandPool(desc), mpDevice(pDevice), mCommandPool(VK_NULL_HANDLE) {
		VkCommandPoolCreateFlags createFlags = 0;
		if (desc.lifetime == CommandListLifetime::ShortLived)
			createFlags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

		uint32_t const queueFamilyIndex = MapQueueTypeToFamilyIndex(pDevice, desc.queueType);

		VkCommandPoolCreateInfo const createInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = createFlags,
			.queueFamilyIndex = queueFamilyIndex
		};

		vkCheckResult(vkCreateCommandPool(pDevice->mDevice, &createInfo, nullptr, &mCommandPool));
	}

	CommandPool::~CommandPool() {
		vkDestroyCommandPool(mpDevice->mDevice, mCommandPool, nullptr);
	}

	ICommandListHandle CommandPool::AllocateCommandList(CommandListDesc const & desc) {
		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

		if (desc.level == CommandListLevel::Direct and not mFreePrimaryBuffers.empty()) {
			commandBuffer = mFreePrimaryBuffers.back();
			mFreePrimaryBuffers.pop_back();
		}
		else if (desc.level == CommandListLevel::Bundle and not mFreeSecondaryBuffers.empty()) {
			commandBuffer = mFreeSecondaryBuffers.back();
			mFreeSecondaryBuffers.pop_back();
		}
		else {
			VkCommandBufferLevel const level = desc.level == CommandListLevel::Direct
				? VK_COMMAND_BUFFER_LEVEL_PRIMARY
				: VK_COMMAND_BUFFER_LEVEL_SECONDARY;

			VkCommandBufferAllocateInfo const allocateInfo{
				.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
				.pNext = nullptr,
				.commandPool = mCommandPool,
				.level = level,
				.commandBufferCount = 1
			};

			vkCheckResult(vkAllocateCommandBuffers(mpDevice->mDevice, &allocateInfo, &commandBuffer));
		}

		if (desc.level == CommandListLevel::Direct) {
			mAllocatedPrimaryBuffers.push_back(commandBuffer);
		}
		else if (desc.level == CommandListLevel::Bundle) {
			mAllocatedSecondaryBuffers.push_back(commandBuffer);
		}

		return new CommandList(desc, commandBuffer);
	}

	void CommandPool::Reset() {
		vkCheckResult(vkResetCommandPool(mpDevice->mDevice, mCommandPool, 0));
		mFreePrimaryBuffers.insert(mFreePrimaryBuffers.end(), std::make_move_iterator(mAllocatedPrimaryBuffers.begin()), std::make_move_iterator(mAllocatedPrimaryBuffers.end()));
		mFreeSecondaryBuffers.insert(mFreeSecondaryBuffers.end(), std::make_move_iterator(mAllocatedSecondaryBuffers.begin()), std::make_move_iterator(mAllocatedSecondaryBuffers.end()));
	}



	CommandList::CommandList(CommandListDesc const & desc, VkCommandBuffer commandBuffer) : ICommandList(desc), mCommandBuffer(commandBuffer) {}

	CommandList::~CommandList() = default;

	void CommandList::BeginCommands(CommandListBeginInfo const & beginInfo) {
		VkCommandBufferUsageFlags flags = 0;
		if (beginInfo.usageFlags & CommandListUsageOneTime) {
			flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		}

		// <todo> Finish the inheritance info for secondary command buffer </todo>
		VkCommandBufferInheritanceInfo const inheritanceInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
			.pNext = nullptr
		};

		VkCommandBufferBeginInfo const vkBeginInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = nullptr,
			.flags = flags,
			.pInheritanceInfo = &inheritanceInfo
		};

		vkCheckResult(vkBeginCommandBuffer(mCommandBuffer, &vkBeginInfo));
	}

	void CommandList::EndCommands() {
		vkCheckResult(vkEndCommandBuffer(mCommandBuffer));
	}

	void CommandList::CopyBuffer(IBuffer * dst, uint64_t dstOffset, IBuffer * src, uint64_t srcOffset, uint64_t numBytes) {
		VkBufferCopy const copy{
			.srcOffset = srcOffset,
			.dstOffset = dstOffset,
			.size = numBytes
		};
		vkCmdCopyBuffer(mCommandBuffer, dynamic_cast<Buffer *>(src)->mBuffer, dynamic_cast<Buffer *>(dst)->mBuffer, 1, &copy);
	}


}
