#include <iostream>

#include "erhi/common/common.hpp"
#include "erhi/common/context/context.hpp"
#include "erhi/common/command/command.hpp"
#include "erhi/common/resource/resource.hpp"
#include "erhi/common/utility/stream_message_callback.hpp"

using namespace erhi;
namespace backend = vk;



void hello_erhi() {
	auto pMessageCallback = std::make_shared<StreamMessageCallback>(std::cout, MessageSeverity::Info);

	DeviceDesc deviceDesc = {
		.enableDebug = true,
		.physicalDevicePreference = PhysicalDevicePreference::HighPerformance,
	};

	auto device = backend::CreateDevice(deviceDesc, pMessageCallback);

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

	auto depthDesc = TextureDesc{
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

	auto depthTexture = device->CreateTexture(MemoryHeapType::Default, depthDesc);

	auto commandPool = device->CreateCommandPool(CommandPoolDesc{
		.queueType = QueueType::Primary,
		.lifetime = CommandListLifetime::ShortLived
	});

	auto commandList = commandPool->AllocateCommandList(CommandListDesc{
		.level = CommandListLevel::Direct
	});

	commandList->BeginCommands({ CommandListUsageOneTime });

	delete commandList;
	delete commandPool;
	delete depthTexture;
	delete renderTarget;
	delete vertexBuffer;
	delete device;
}



int main() {
	try {
		hello_erhi();
	}
	catch (std::exception const & e) {
		std::cout << e.what();
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