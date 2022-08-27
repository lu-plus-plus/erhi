#ifndef ERHI_DEVICE_HPP
#define ERHI_DEVICE_HPP

#include "handle.hpp"
#include "object.hpp"



namespace erhi {

	struct DeviceDesc {
		IPhysicalDeviceHandle physicalDeviceHandle;
	};

	struct IDevice : IObject {
		IDevice(DeviceDesc const & desc);
		~IDevice();
	};

}



#endif // ERHI_DEVICE_HPP