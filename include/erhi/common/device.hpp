#ifndef ERHI_DEVICE_HPP
#define ERHI_DEVICE_HPP

#include "handle.hpp"
#include "object.hpp"



namespace erhi {

	struct IDevice : IObject {
		IDevice(IPhysicalDeviceHandle physicalDeviceHandle);
		~IDevice();

		IQueueHandle createQueue();
	};

}



#endif // ERHI_DEVICE_HPP