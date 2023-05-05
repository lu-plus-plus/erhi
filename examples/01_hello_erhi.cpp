#include <format>
#include <iostream>

#include "erhi/common/common.hpp"
#include "erhi/common/context/context.hpp"

// <todo> resource.hpp </todo>
#include "erhi/common/resource/memory.hpp"

// <todo> command.hpp </todo>
#include "erhi/common/command/queue.hpp"

#include "utility/default_message_callback.hpp"

#include "erhi/common/exception.hpp"

using namespace erhi;



int main() {
	try {
		auto pInstance = vk::createInstance(InstanceDesc{
			.enableDebug = true,
			.pMessageCallback = MakeHandle<DefaultMessageCallback>(MessageSeverity::Info)
			});

		auto pPhysicalDevice = pInstance->selectPhysicalDevice(PhysicalDeviceDesc{
			.type = PhysicalDeviceType::Discrete
			});

		auto pDevice = pPhysicalDevice->createDevice(DeviceDesc{});

		auto pPrimaryQueue = pDevice->SelectQueue(QueueType::Primary);

		auto indexBufferDesc = BufferDesc{
			.usage = BufferUsageCopySource | BufferUsageCopyTarget | BufferUsageIndexBuffer,
			.size = 2 * 3 * sizeof(uint32_t)
		};

		auto indexBuffer = pDevice->CreateCommittedBuffer(MemoryHeapType::Default, indexBufferDesc);

		auto renderTargetDesc = TextureDesc{
			.dimension = TextureDimension::Texture2D,
			.extent = { 1920u, 1080u, 1u },
			.format = Format::R32G32B32A32_Float,
			.mipLevels = 1u,
			.sampleCount = TextureSampleCount::Count_1,
			.usage = TextureUsageRenderTargetAttachment,
			.tiling = TextureTiling::Optimal
		};

		auto renderTarget = pDevice->CreateCommittedTexture(MemoryHeapType::Default, renderTargetDesc);

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
		std::cout << e.what() << std::endl;
	}
	catch (erhi::base_exception const & e) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}