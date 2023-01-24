#include <format>

#include "erhi/common/common.hpp"
#include "erhi/common/context/context.hpp"

// <todo> resource.hpp </todo>
#include "erhi/common/resource/memory.hpp"

// <todo> command.hpp </todo>
#include "erhi/common/command/queue.hpp"

#include "utility/default_message_callback.hpp"

using namespace erhi;



int main() {
	auto pInstance = vk::createInstance(InstanceDesc{
		.enableDebug = true,
		.pMessageCallback = MakeHandle<DefaultMessageCallback>(MessageSeverity::Info)
	});

	auto pPhysicalDevice = pInstance->selectPhysicalDevice(PhysicalDeviceDesc{
		.type = PhysicalDeviceType::Discrete
	});

	auto pDevice = pPhysicalDevice->createDevice(DeviceDesc{});

	auto pPrimaryQueue = pDevice->SelectQueue(QueueType::Graphics);

	auto indexBufferDesc = BufferDesc{
		.bufferUsage = BufferUsageBits::CopySource | BufferUsageBits::CopyTarget | BufferUsageBits::IndexBuffer,
		.size = 12 * 3 * sizeof(uint32_t)
	};

	auto memoryRequirements = pDevice->GetBufferMemoryRequirements(
		MemoryHeapType::Default,
		indexBufferDesc
	);

	auto indexBufferMemory = pDevice->AllocateMemory(MemoryDesc{
		.memoryTypeIndex = memoryRequirements.memoryTypeIndex,
		.size = memoryRequirements.size
	});

	auto indexBuffer = pDevice->CreateBuffer(indexBufferMemory, 0, indexBufferDesc);

	return 0;
}