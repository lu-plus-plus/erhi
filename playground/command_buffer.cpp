#include "erhi/vulkan/context/device.hpp"

#include "erhi/vulkan/command/command_buffer.hpp"

#include "vulkan/vulkan.hpp"



namespace erhi::vk {

	VulkanCommandPool::VulkanCommandPool(VkDevice device, VkCommandPoolCreateFlags poolCreateInfoFlags, uint32_t queueFamilyIndex) : mDevice(device), mCommandPool(VK_NULL_HANDLE), mFreeBuffers(), mAllocatedBuffers() {
		VkCommandPoolCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = poolCreateInfoFlags,
			.queueFamilyIndex = queueFamilyIndex
		};
		vkCheckResult(vkCreateCommandPool(mDevice, &createInfo, nullptr, &mCommandPool));
	}

	VulkanCommandPool::~VulkanCommandPool() {
		vkDestroyCommandPool(mDevice, mCommandPool, nullptr);
	}

	VkCommandBuffer VulkanCommandPool::AllocateCommandBuffer(VkCommandBufferLevel level) {
		VkCommandBufferAllocateInfo allocateInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext = nullptr,
			.commandPool = mCommandPool,
			.level = level,
			.commandBufferCount = 1u
		};
		VkCommandBuffer commandBuffer;
		vkCheckResult(vkAllocateCommandBuffers(mDevice, &allocateInfo, &commandBuffer));
		return commandBuffer;
	}

	void VulkanCommandPool::Reset() {
		// VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT seems unnecessary unless the size of command pool increases monotonically.
		VkCommandPoolResetFlags flags = 0;
		vkCheckResult(vkResetCommandPool(mDevice, mCommandPool, flags));
	}



	CommandPool::CommandPool(Device * pDevice, CommandPoolDesc const & desc) : ICommandPool(desc), mDeviceHandle(pDevice), mPoolCreateFlags(0u) {
		if (desc.lifetime == CommandListLifetime::ShortLived) {
			mPoolCreateFlags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		}
	}

	CommandPool::~CommandPool() = default;

	ICommandListHandle CommandPool::AllocateCommandList(CommandListDesc const & desc) {
		std::optional<VulkanCommandPool> * pPool = nullptr;
		uint32_t queueFamilyIndex = std::numeric_limits<uint32_t>::max();

		if (desc.queueType == QueueType::Primary) {
			pPool = &mPrimaryCommandPool;
			queueFamilyIndex = mDeviceHandle->mGraphicsQueueFamilyIndex;
		}
		else if (desc.queueType == QueueType::AsyncCompute) {
			pPool = &mComputeCommandPool;
			queueFamilyIndex = mDeviceHandle->mComputeQueueFamilyIndex.value();
		}
		else if (desc.queueType == QueueType::AsyncCopy) {
			pPool = &mCopyCommandPool;
			queueFamilyIndex = mDeviceHandle->mCopyQueueFamilyIndex.value();
		}

		if (not *pPool) pPool->emplace(mDeviceHandle->mDevice, mPoolCreateFlags, queueFamilyIndex);

		VkCommandBufferLevel level =
			desc.level == CommandListLevel::Direct
			? VK_COMMAND_BUFFER_LEVEL_PRIMARY
			: VK_COMMAND_BUFFER_LEVEL_SECONDARY;

		VkCommandBuffer commandBuffer = pPool->value().AllocateCommandBuffer(level);

		return MakeHandle<CommandList>(desc, commandBuffer);
	}

	void CommandPool::Reset() {
		if (mPrimaryCommandPool) mPrimaryCommandPool.value().Reset();
		if (mComputeCommandPool) mComputeCommandPool.value().Reset();
		if (mCopyCommandPool) mCopyCommandPool.value().Reset();
	}



	CommandList::CommandList(CommandListDesc const & desc, VkCommandBuffer commandBuffer) : ICommandList(desc), mCommandBuffer(commandBuffer) {}

	CommandList::~CommandList() = default;

	void CommandList::BeginCommands() {
		VkCommandBufferBeginInfo info{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = nullptr,
			.flags = m
		};

		::vk::CommandBufferBeginInfo info(
			VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		)
		vkCheckResult(vkBeginCommandBuffer(mCommandBuffer, ));
	}

}