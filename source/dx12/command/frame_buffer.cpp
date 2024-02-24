#include "erhi/dx12/context/context.hpp"
#include "erhi/dx12/command/command.hpp"
#include "erhi/dx12/resource/resource.hpp"

#include <cassert>

namespace erhi::dx12 {

	FrameBuffer::FrameBuffer(DeviceHandle pDevice, FrameBufferDesc const & desc) : IFrameBuffer(desc), mpDevice(pDevice) {
		D3D12_DESCRIPTOR_HEAP_DESC const rtvHeapDesc = {
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			.NumDescriptors = UINT(desc.pRenderPass->mRenderTargetAttachments.size()),
			.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			.NodeMask = 0
		};
		D3D12CheckResult(pDevice->mpDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(mRenderTargetViews.GetAddressOf())));

		for (auto i = 0u; i < desc.pRenderPass->mRenderTargetAttachments.size(); ++i) {
			uint32_t const renderTargetAttachmentIndex = desc.pRenderPass->mRenderTargetAttachments[i];

			if (renderTargetAttachmentIndex >= desc.attachmentCount) {
				// <todo> unattached render target may be allowed </todo>
				continue;
			}
			
			auto const & attachment = desc.pRenderPass->mAttachments[renderTargetAttachmentIndex];
			auto pTextureView = dynamic_cast<TextureViewHandle>(desc.attachments[renderTargetAttachmentIndex]);

			assert(pTextureView->mDesc.format == attachment.format);

			D3D12_RENDER_TARGET_VIEW_DESC const rtvDesc = {
				.Format = mapping::MapFormat(attachment.format),
				.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D,
				.Texture2D = { .MipSlice = 0, .PlaneSlice = 0 }
			};

			CD3DX12_CPU_DESCRIPTOR_HANDLE const descriptor(
				mRenderTargetViews->GetCPUDescriptorHandleForHeapStart(),
				i,
				mpDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
			);

			pDevice->mpDevice->CreateRenderTargetView(pTextureView->mpTexture->mpResource.Get(), &rtvDesc, descriptor);
		}

		if (desc.pRenderPass->mDepthStencilAttachment) {
			D3D12_DESCRIPTOR_HEAP_DESC const dsvHeapDesc = {
				.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
				.NumDescriptors = 1,
				.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
				.NodeMask = 0
			};
			D3D12CheckResult(pDevice->mpDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(mDepthStencilViews.GetAddressOf())));

			uint32_t const depthStencilAttachmentIndex = desc.pRenderPass->mDepthStencilAttachment.value();

			auto const & attachment = desc.pRenderPass->mAttachments[depthStencilAttachmentIndex];
			auto pTextureView = dynamic_cast<TextureViewHandle>(desc.attachments[depthStencilAttachmentIndex]);

			assert(pTextureView->mDesc.format == attachment.format);

			D3D12_DEPTH_STENCIL_VIEW_DESC const dsvDesc = {
				.Format = mapping::MapFormat(attachment.format),
				.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,
				.Texture2D = { .MipSlice = 0 }
			};

			CD3DX12_CPU_DESCRIPTOR_HANDLE const descriptor(
				mDepthStencilViews->GetCPUDescriptorHandleForHeapStart(),
				0,
				mpDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
			);

			pDevice->mpDevice->CreateDepthStencilView(pTextureView->mpTexture->mpResource.Get(), &dsvDesc, descriptor);
		}
	}

	FrameBuffer::~FrameBuffer() = default;

	IFrameBufferHandle Device::CreateFrameBuffer(FrameBufferDesc const & desc) {
		return new FrameBuffer(this, desc);
	}

}