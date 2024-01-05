#include "erhi/common/context/context.hpp"



namespace erhi {

	IDevice::IDevice(DeviceDesc const & desc) : mDesc(desc), mpMessageCallback(nullptr) {
		mpMessageCallback = GetPhysicalDevice()->GetInstance()->mpMessageCallback;
	}
	
	IDevice::~IDevice() = default;

	IMessageCallback & IDevice::Log() const {
		return *mpMessageCallback;
	}

}