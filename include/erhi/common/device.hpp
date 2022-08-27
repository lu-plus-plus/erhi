#ifndef ERHI_DEVICE_HPP
#define ERHI_DEVICE_HPP

#include "common.hpp"



namespace erhi {

	struct IDevice : IObject {
		IDevice(IPhysicalDeviceHandle physicalDeviceHandle);
		~IDevice();

		virtual IQueueHandle selectQueue(QueueType queueType) = 0;
	};

}



#endif // ERHI_DEVICE_HPP