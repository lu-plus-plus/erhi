#include "erhi/common/context/message.hpp"
#include "erhi/common/context/instance.hpp"
#include "erhi/common/context/physical_device.hpp"
#include "erhi/common/context/device.hpp"



namespace erhi {

	IDevice::IDevice() = default;
	
	IDevice::~IDevice() = default;

	void IDevice::Log(MessageType type, MessageSeverity severity, std::string_view message) {
		(*GetPhysicalDevice()->pInstance()->mMessageCallbackHandle)(type, severity, message.data());
	}
	void IDevice::Verbose(std::string_view message) { GetPhysicalDevice()->pInstance()->mMessageCallbackHandle->verbose(message.data()); }
	void IDevice::Info(std::string_view message) { GetPhysicalDevice()->pInstance()->mMessageCallbackHandle->info(message.data()); }
	void IDevice::Warning(std::string_view message) { GetPhysicalDevice()->pInstance()->mMessageCallbackHandle->warning(message.data()); }
	void IDevice::Error(std::string_view message) { GetPhysicalDevice()->pInstance()->mMessageCallbackHandle->error(message.data()); }

}