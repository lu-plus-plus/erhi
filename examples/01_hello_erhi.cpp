#include <iostream>

#include "erhi/common/common.hpp"
#include "erhi/common/context/context.hpp"
#include "erhi/common/present/present.hpp"
#include "erhi/common/command/command.hpp"
#include "erhi/common/resource/resource.hpp"

#include "erhi/common/utility/stream_message_callback.hpp"

using namespace erhi;
namespace backend = vk;



struct WindowMessageCallback : IWindowMessageCallback
{
	void OnRender() override {}
};

void hello_erhi() {
	auto pMessageCallback = std::make_shared<StreamMessageCallback>(std::cout, MessageSeverity::Warning);
	
	DeviceDesc deviceDesc = {
		.enableDebug = true,
		.physicalDevicePreference = PhysicalDevicePreference::HighPerformance,
	};

	auto device = backend::CreateDevice(deviceDesc, pMessageCallback);

	WindowDesc windowDesc = {
		.width = 1920,
		.height = 1080,
		.left = 64,
		.top = 64,
		.windowName = "hello eRHI",
		.pMessageCallback = WindowMessageCallback()
	};
	
	auto window = device->CreateNewWindow(windowDesc);

	SwapChainDesc swapChainDesc = {
		.pWindow = window,
		.format = Format::B8G8R8A8_UNorm,
		.bufferCount = 3,
		.usageFlags = TextureUsageRenderTarget
	};

	auto swapChain = window->CreateSwapChain(swapChainDesc);

	auto presentTexture = swapChain->GetTexture(0);

 	auto primaryQueue = device->SelectQueue(QueueType::Primary);

	auto vertexBufferDesc = BufferDesc{
		.usage = BufferUsageCopyTarget | BufferUsageVertexBuffer,
		.size = 4 * 3 * sizeof(float)
	};

	auto vertexBuffer = device->CreateBuffer(MemoryHeapType::Default, vertexBufferDesc);

	auto renderTargetDesc = TextureDesc{
		.dimension = TextureDimension::Texture2D,
		.extent = { 1920u, 1080u, 1u },
		.format = Format::R32G32B32A32_Float,
		.mipLevels = 1u,
		.sampleCount = TextureSampleCount::Count_1,
		.usage = TextureUsageRenderTarget,
		.tiling = TextureTiling::Optimal,
		.initialLayout = TextureLayout::RenderTarget,
		.initialQueueType = QueueType::Primary
	};

	auto renderTarget = device->CreateTexture(MemoryHeapType::Default, renderTargetDesc);

	auto renderTargetView = device->CreateTextureView(renderTarget, TextureViewDesc{
		.dimension = TextureViewDimension::Texture2D,
		.format = renderTargetDesc.format,
		.mostDetailedMipLevel = 0,
		.mipLevelCount = 1,
		.aspectFlags = TextureAspectColor
	});

	auto depthStencilDesc = TextureDesc{
		.dimension = TextureDimension::Texture2D,
		.extent = { 1920u, 1080u, 1u },
		.format = Format::D32_Float,
		.mipLevels = 1u,
		.sampleCount = TextureSampleCount::Count_1,
		.usage = TextureUsageDepthStencil,
		.tiling = TextureTiling::Optimal,
		.initialLayout = TextureLayout::DepthStencilWrite,
		.initialQueueType = QueueType::Primary
	};

	auto depthStencil = device->CreateTexture(MemoryHeapType::Default, depthStencilDesc);

	auto depthStencilView = device->CreateTextureView(depthStencil, TextureViewDesc{
		.dimension = TextureViewDimension::Texture2D,
		.format = depthStencilDesc.format,
		.mostDetailedMipLevel = 0,
		.mipLevelCount = 1,
		.aspectFlags = TextureAspectDepth
	});

	AttachmentDesc renderTargetAttachment = {
		.format = renderTargetDesc.format,
		.sampleCount = renderTargetDesc.sampleCount,
		.loadOp = AttachmentLoadOp::Clear,
		.storeOp = AttachmentStoreOp::Store,
		.stencilLoadOp = AttachmentLoadOp::DoNotCare,
		.stencilStoreOp = AttachmentStoreOp::DoNotCare,
		.initialLayout = TextureLayout::RenderTarget,
		.subpassLayout = TextureLayout::RenderTarget,
		.finalLayout = TextureLayout::CopySource
	};

	AttachmentDesc depthStencilAttachment = {
		.format = depthStencilDesc.format,
		.sampleCount = depthStencilDesc.sampleCount,
		.loadOp = AttachmentLoadOp::Clear,
		.storeOp = AttachmentStoreOp::DoNotCare,
		.stencilLoadOp = AttachmentLoadOp::DoNotCare,
		.stencilStoreOp = AttachmentStoreOp::DoNotCare,
		.initialLayout = TextureLayout::DepthStencilWrite,
		.subpassLayout = TextureLayout::DepthStencilWrite,
		.finalLayout = TextureLayout::DepthStencilWrite
	};

	std::vector<AttachmentDesc> attachments = {
		renderTargetAttachment,
		depthStencilAttachment
	};

	uint32_t renderTargetAttachementIndex = 0;
	uint32_t depthStencilAttachementIndex = 1;

	RenderPassDesc renderPassDesc = {
		.pipelineBindPoint = PipelineBindPoint::Graphics,
		.attachmentCount = uint32_t(attachments.size()),
		.attachments = attachments.data(),
		.renderTargetAttachmentCount = 1,
		.renderTargetAttachments = &renderTargetAttachementIndex,
		.pDepthStencilAttachment = &depthStencilAttachementIndex
	};

	auto renderPass = device->CreateRenderPass(renderPassDesc);

	std::vector<ITextureViewHandle> frameBufferAttachments = {
		renderTargetView,
		depthStencilView
	};

	FrameBufferDesc frameBufferDesc = {
		.pRenderPass = renderPass,
		.attachmentCount = uint32_t(frameBufferAttachments.size()),
		.attachments = frameBufferAttachments.data(),
		.width = renderTargetDesc.extent[0],
		.height = renderTargetDesc.extent[1]
	};

	auto frameBuffer = device->CreateFrameBuffer(frameBufferDesc);

	auto commandPool = device->CreateCommandPool(CommandPoolDesc{
		.queueType = QueueType::Primary,
		.lifetime = CommandListLifetime::ShortLived
	});

	auto commandList = commandPool->AllocateCommandList(CommandListDesc{
		.level = CommandListLevel::Direct
	});

	commandList->BeginCommands({ .usageFlags = CommandListUsageOneTime });

	commandList->SetPrimitiveTopology(PrimitiveTopology::TriangleList);

	VertexBufferView vertexBufferView{
		.pBuffer = vertexBuffer,
		.offset = 0,
		.size = vertexBuffer->mDesc.size,
		.stride = 12
	};

	commandList->BindVertexBuffers(0, 1, &vertexBufferView);

	commandList->EndCommands();

	delete commandList;
	delete commandPool;
	delete renderPass;
	delete frameBuffer;
	delete depthStencilView;
	delete depthStencil;
	delete renderTargetView;
	delete renderTarget;
	delete vertexBuffer;
	delete swapChain;
	delete window;
	delete device;
}



int main() {
	try {
		hello_erhi();
	}
	catch (std::exception const & e) {
		std::cout << e.what() << '\n';
		std::cout << "TERMINATED on exception.\n";
		std::terminate();
	}
	catch (...) {
		std::cout << "An unknown exception is thrown.\n";
		std::cout << "TERMINATED on exception.\n";
		std::terminate();
	}

	return 0;
}