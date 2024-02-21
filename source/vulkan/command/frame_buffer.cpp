#include "erhi/vulkan/context/context.hpp"
#include "erhi/vulkan/command/command.hpp"
#include "erhi/vulkan/resource/resource.hpp"

namespace erhi::vk {

	FrameBuffer::FrameBuffer(DeviceHandle pDevice, FrameBufferDesc const & desc) : IFrameBuffer(desc), mpDevice(pDevice), mFrameBuffer(VK_NULL_HANDLE) {
		std::vector<VkImageView> attachments;
		attachments.reserve(desc.attachmentCount);
		for (size_t i = 0; i < desc.attachmentCount; ++i)
			attachments.push_back(dynamic_cast<TextureView const *>(desc.attachments)->mImageView);
		
		VkFramebufferCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.renderPass = dynamic_cast<RenderPassHandle>(desc.pRenderPass)->mRenderPass,
			.attachmentCount = desc.attachmentCount,
			.pAttachments = attachments.data(),
			.width = desc.width,
			.height = desc.height,
			.layers = 1
		};

		vkCheckResult(vkCreateFramebuffer(*mpDevice, &createInfo, nullptr, &mFrameBuffer));
	}

	FrameBuffer::~FrameBuffer() {
		vkDestroyFramebuffer(*mpDevice, mFrameBuffer, nullptr);
	}

}