#include <format>

#include "erhi/common/common.hpp"
#include "erhi/common/message.hpp"
#include "erhi/common/instance.hpp"
#include "erhi/common/physical_device.hpp"
#include "erhi/common/device.hpp"
#include "erhi/common/queue.hpp"

#include "utility/default_message_callback.hpp"

using namespace erhi;



int main() {

	auto pInstance = vk::createInstance(InstanceDesc{
		.enableDebug = true,
		.pMessageCallback = MakeHandle<DefaultMessageCallback>(MessageSeverity::Info)
	});

	auto pPhysicalDevice = pInstance->selectPhysicalDevice(PhysicalDeviceDesc{
		.type = PhysicalDeviceType::Integrated
	});

	auto pDevice = pPhysicalDevice->createDevice(DeviceDesc{});

	auto pPrimaryQueue = pDevice->selectQueue(QueueType::Primary);

	return 0;
}