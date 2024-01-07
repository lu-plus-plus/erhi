#include <iostream>

#include "erhi/common/common.hpp"
#include "erhi/common/context/context.hpp"
#include "erhi/common/resource/resource.hpp"
#include "erhi/common/utility/stram_message_callback.hpp"

using namespace erhi;
namespace backend = vk;



int main() {
	try {
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
			.usage = TextureUsageRenderTargetAttachment,
			.tiling = TextureTiling::Optimal
		};

		auto renderTarget = pDevice->CreateTexture(MemoryHeapType::Default, renderTargetDesc);

		//auto memoryRequirements = pDevice->GetBufferMemoryRequirements(
		//	MemoryHeapType::Default,
		//	indexBufferDesc
		//);

		//if (memoryRequirements.memoryTypeBits == 0u) {
		//	throw std::exception("Failed to find any compatible memory type.\n");
		//}

		//auto indexBufferMemory = pDevice->AllocateMemory(MemoryDesc{
		//	.memoryTypeIndex = uint32_t(std::countr_zero(memoryRequirements.memoryTypeBits)),
		//	.size = memoryRequirements.size
		//	});

		//auto indexBuffer = pDevice->CreatePlacedBuffer(indexBufferMemory, 0, memoryRequirements.alignment, indexBufferDesc);
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