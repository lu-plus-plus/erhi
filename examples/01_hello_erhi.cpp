#include <format>

#include "erhi/common/message.hpp"
#include "erhi/common/instance.hpp"
#include "erhi/common/physical_device.hpp"
#include "erhi/common/device.hpp"

#include "utility/default_message_callback.hpp"

using namespace erhi;

using namespace erhi::vk;



int main() {

	auto pInstance{ createInstance(InstanceDesc{
		.enableDebug = true,
		.pMessageCallback = MakeHandle<DefaultMessageCallback>(MessageSeverity::Info)
	}) };

	auto pPhysicalDevice{ pInstance->selectDefaultPhysicalDevice() };

	auto pDevice{ pPhysicalDevice->createDevice() };

	return 0;
}