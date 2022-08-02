#include <format>

#include "erhi/common/instance.hpp"
#include "erhi/common/physical_device.hpp"
#include "erhi/common/message.hpp"

#include "utility/default_message_callback.hpp"

using namespace erhi;

using namespace erhi::vk;



int main() {

	InstanceHandle pInstance{ createInstance(InstanceDesc{
		.enableDebug = true,
		.pMessageCallback = create<DefaultMessageCallback>(MessageSeverity::Verbose)
	}) };

	PhysicalDeviceHandle pPhysicalDevice{ pInstance->selectDefaultPhysicalDevice() };

	return 0;
}