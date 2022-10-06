#include <format>

#include "erhi/common/common.hpp"
#include "erhi/common/context.hpp"

#include "utility/default_message_callback.hpp"

using namespace erhi;



int main() {
	auto pInstance = dx12::createInstance(InstanceDesc{
		.enableDebug = true,
		.pMessageCallback = MakeHandle<DefaultMessageCallback>(MessageSeverity::Info)
	});

	auto pPhysicalDevice = pInstance->selectPhysicalDevice(PhysicalDeviceDesc{
		.type = PhysicalDeviceType::Discrete
	});

	auto pDevice = pPhysicalDevice->createDevice(DeviceDesc{});

	auto pPrimaryQueue = pDevice->selectQueue(QueueType::Primary);

	return 0;
}