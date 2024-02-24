#include "erhi/vulkan/context/context.hpp"
#include "erhi/vulkan/command/command.hpp"

#include <vector>

namespace erhi::vk
{
	VkAttachmentDescription MapAttachmentDescription(AttachmentDesc const & desc) {
		VkAttachmentDescription const vulkanDesc = {
			.flags = 0,
			.format = mapping::MapFormat(desc.format),
			.samples = mapping::MapTextureSampleCount(desc.sampleCount),
			.loadOp = mapping::MapAttachmentLoadOp(desc.loadOp),
			.storeOp = mapping::MapAttachmentStoreOp(desc.storeOp),
			.stencilLoadOp = mapping::MapAttachmentLoadOp(desc.stencilLoadOp),
			.stencilStoreOp = mapping::MapAttachmentStoreOp(desc.stencilStoreOp),
			.initialLayout = mapping::MapTextureLayout(desc.initialLayout),
			.finalLayout = mapping::MapTextureLayout(desc.finalLayout),
		};
		return vulkanDesc;
	}

	RenderPass::RenderPass(DeviceHandle pDevice, RenderPassDesc const & desc) :
		IRenderPass(desc), mpDevice(pDevice), mRenderPass(VK_NULL_HANDLE) {
		
		std::vector<VkAttachmentDescription> attachments;
		attachments.reserve(desc.attachmentCount);
		for (uint32_t i = 0; i < desc.attachmentCount; ++i) {
			attachments.push_back(MapAttachmentDescription(desc.attachments[i]));
		}

		std::vector<VkAttachmentReference> colorAttachmentReferences;
		colorAttachmentReferences.reserve(desc.renderTargetAttachmentCount);
		for (uint32_t i = 0; i < desc.renderTargetAttachmentCount; ++i) {
			uint32_t const index = desc.renderTargetAttachments[i];
			VkAttachmentReference const ref = {
				.attachment = index,
				.layout = mapping::MapTextureLayout(desc.attachments[index].subpassLayout)
			};
			colorAttachmentReferences.push_back(ref);
		}

		VkAttachmentReference depthStencilReference = {
			.attachment = VK_ATTACHMENT_UNUSED,
			.layout = VK_IMAGE_LAYOUT_UNDEFINED
		};

		if (desc.pDepthStencilAttachment) {
			uint32_t const index = *desc.pDepthStencilAttachment;
			depthStencilReference.attachment = index;
			depthStencilReference.layout = mapping::MapTextureLayout(desc.attachments[index].subpassLayout);
		}

		VkSubpassDescription const subpass = {
			.flags = 0,
			.pipelineBindPoint = mapping::MapPipelineBindPoint(desc.pipelineBindPoint),
			.inputAttachmentCount = 0,
			.pInputAttachments = nullptr,
			.colorAttachmentCount = uint32_t(colorAttachmentReferences.size()),
			.pColorAttachments = colorAttachmentReferences.data(),
			.pDepthStencilAttachment = &depthStencilReference,
			.preserveAttachmentCount = 0,
			.pPreserveAttachments = nullptr
		};

		VkRenderPassCreateInfo const renderPassCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.attachmentCount = uint32_t(attachments.size()),
			.pAttachments = attachments.data(),
			.subpassCount = 1,
			.pSubpasses = &subpass,
			.dependencyCount = 0,
			.pDependencies = nullptr
		};

		vkCheckResult(vkCreateRenderPass(*pDevice, &renderPassCreateInfo, nullptr, &mRenderPass));
	}

	RenderPass::~RenderPass() {
		vkDestroyRenderPass(*mpDevice, mRenderPass, nullptr);
	}

	IRenderPassHandle Device::CreateRenderPass(RenderPassDesc const & desc) {
		return new RenderPass(this, desc);
	}
}