#include "erhi/common/context/context.hpp"



namespace erhi {

	IDevice::IDevice(DeviceDesc const & desc, std::shared_ptr<IMessageCallback> pMessageCallback) : mDesc(desc), mpMessageCallback(pMessageCallback) {}
	
	IDevice::~IDevice() = default;

	IMessageCallback & IDevice::Log() const {
		return *mpMessageCallback;
	}

}