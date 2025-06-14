#include <iostream>
#include <source_location>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "erhi/common/common.hpp"
#include "erhi/common/context/context.hpp"
#include "erhi/common/present/present.hpp"
#include "erhi/common/command/command.hpp"
#include "erhi/common/resource/resource.hpp"

#include "erhi/common/utility/smart_ptr.hpp"
#include "erhi/common/utility/stream_message_callback.hpp"

namespace backend = erhi::vk;

using namespace erhi;



struct WindowMessageCallback : IWindowMessageCallback
{
	void OnRender() override {}
};



void hello_erhi() {
	auto pMessageCallback = std::make_shared<StreamMessageCallback>(std::cout, MessageSeverity::Info);
	
	DeviceDesc deviceDesc = {
		.enableDebug = true,
		.physicalDevicePreference = PhysicalDevicePreference::HighPerformance,
	};

	auto device = to_unique(backend::CreateDevice(deviceDesc, pMessageCallback));

	WindowDesc windowDesc = {
		.width = 1920,
		.height = 1080,
		.left = 64,
		.top = 64,
		.windowName = "hello eRHI",
		.pMessageCallback = WindowMessageCallback()
	};
	
	auto window = to_unique(device->CreateNewWindow(windowDesc));

	SwapChainDesc swapChainDesc = {
		.pWindow = window.get(),
		.format = Format::B8G8R8A8_UNorm,
		.bufferCount = 3,
		.usageFlags = TextureUsageRenderTarget
	};

	auto swapChain = to_unique(window->CreateSwapChain(swapChainDesc));

	auto presentTexture = to_unique(swapChain->GetTexture(0));

 	auto primaryQueue = device->SelectQueue(QueueType::Primary);

	auto vertexBufferDesc = BufferDesc{
		.usage = BufferUsageCopyTarget | BufferUsageVertexBuffer,
		.size = 4 * 3 * sizeof(float)
	};

	auto vertexBuffer = to_unique(device->CreateBuffer(MemoryHeapType::Default, vertexBufferDesc));

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

	auto renderTarget = to_unique(device->CreateTexture(MemoryHeapType::Default, renderTargetDesc));

	auto renderTargetView = to_unique(device->CreateTextureView(renderTarget.get(), TextureViewDesc{
		.dimension = TextureViewDimension::Texture2D,
		.format = renderTargetDesc.format,
		.mostDetailedMipLevel = 0,
		.mipLevelCount = 1,
		.aspectFlags = TextureAspectColor
	}));

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

	auto depthStencil = to_unique(device->CreateTexture(MemoryHeapType::Default, depthStencilDesc));

	auto depthStencilView = to_unique(device->CreateTextureView(depthStencil.get(), TextureViewDesc{
		.dimension = TextureViewDimension::Texture2D,
		.format = depthStencilDesc.format,
		.mostDetailedMipLevel = 0,
		.mipLevelCount = 1,
		.aspectFlags = TextureAspectDepth
	}));

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

	auto renderPass = to_unique(device->CreateRenderPass(renderPassDesc));

	std::vector<ITextureViewHandle> frameBufferAttachments = {
		renderTargetView.get(),
		depthStencilView.get()
	};

	FrameBufferDesc frameBufferDesc = {
		.pRenderPass = renderPass.get(),
		.attachmentCount = uint32_t(frameBufferAttachments.size()),
		.attachments = frameBufferAttachments.data(),
		.width = renderTargetDesc.extent[0],
		.height = renderTargetDesc.extent[1]
	};

	auto frameBuffer = to_unique(device->CreateFrameBuffer(frameBufferDesc));

	auto commandPool = to_unique(device->CreateCommandPool(CommandPoolDesc{
		.queueType = QueueType::Primary,
		.lifetime = CommandListLifetime::ShortLived
	}));

	auto commandList = to_unique(commandPool->AllocateCommandList(CommandListDesc{
		.level = CommandListLevel::Direct
	}));

	commandList->BeginCommands({ .usageFlags = CommandListUsageOneTime });

	commandList->SetPrimitiveTopology(PrimitiveTopology::TriangleList);

	auto const vertexBufferView = VertexBufferView{
		.pBuffer = vertexBuffer.get(),
		.offset = 0,
		.size = vertexBuffer->mDesc.size,
		.stride = 12
	};

	commandList->BindVertexBuffers(0, 1, &vertexBufferView);

	commandList->EndCommands();

	std::filesystem::path const thisPath = std::source_location::current().file_name();
	auto const shaderPath = thisPath.parent_path().parent_path() / "shaders" / "01_hello_erhi.hlsl";
	if (not std::filesystem::exists(shaderPath)) {
		throw std::runtime_error("shader path " + shaderPath.string() + " does not exist");
	}

	auto LoadFileAsString = [] (std::filesystem::path const & path) -> std::string {
		auto fileStream = std::ifstream(path);
		auto stringStream = std::stringstream();
		stringStream << fileStream.rdbuf();
		return stringStream.str();
	};

	auto const shaderSource = LoadFileAsString(shaderPath);

	auto shaderCompiler = to_unique(device->CreateShaderCompiler(ShaderCompilerDesc{}));
	
	ShaderCompileInfo const vertexShaderCompileInfo = {
		.sourceSizeInBytes = uint32_t(shaderSource.size()),
		.sourceCode = shaderSource.data(),
		.shaderType = ShaderType::Vertex,
		.entryPoint = L"VSMain",
		.enableDebug = true,
		.pMessageCallback = pMessageCallback.get()
	};

	auto vertexShaderBlob = to_unique(shaderCompiler->Compile(vertexShaderCompileInfo));

	ShaderCompileInfo const pixelShaderCompileInfo = {
		.sourceSizeInBytes = uint32_t(shaderSource.size()),
		.sourceCode = shaderSource.data(),
		.shaderType = ShaderType::Pixel,
		.entryPoint = L"PSMain",
		.enableDebug = true,
		.pMessageCallback = pMessageCallback.get()
	};

	auto pixelShaderBlob = to_unique(shaderCompiler->Compile(pixelShaderCompileInfo));
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