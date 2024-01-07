#include <iostream>

#include "erhi/common/common.hpp"
#include "erhi/common/context/context.hpp"
#include "erhi/common/resource/resource.hpp"
#include "erhi/common/utility/stream_message_callback.hpp"

using namespace erhi;
namespace backend = dx12;



void hello_erhi() {
	auto pMessageCallback = std::make_shared<StreamMessageCallback>(std::cout, MessageSeverity::Info);

	DeviceDesc deviceDesc = {
		.enableDebug = true,
		.physicalDevicePreference = PhysicalDevicePreference::HighPerformance,
	};

	auto pDevice = backend::CreateDevice(deviceDesc, pMessageCallback);

	auto pPrimaryQueue = pDevice->SelectQueue(QueueType::Primary);

	auto indexBufferDesc = BufferDesc{
		.usage = BufferUsageCopyTarget | BufferUsageIndexBuffer,
		.size = 2 * 3 * sizeof(uint32_t)
	};

	auto indexBuffer = pDevice->CreateBuffer(MemoryHeapType::Default, indexBufferDesc);

	auto vertexBufferDesc = BufferDesc{
		.usage = BufferUsageCopyTarget | BufferUsageVertexBuffer,
		.size = 4 * 3 * sizeof(float)
	};

	auto vertexBuffer = pDevice->CreateBuffer(MemoryHeapType::Default, vertexBufferDesc);

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

	auto renderTarget = pDevice->CreateTexture(MemoryHeapType::Default, renderTargetDesc);

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