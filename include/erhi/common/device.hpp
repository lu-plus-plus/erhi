#ifndef ERHI_DEVICE_HPP
#define ERHI_DEVICE_HPP

#include "common.hpp"



namespace erhi {

	struct IDevice : IObject {
		IDevice();
		virtual ~IDevice();

		virtual IPhysicalDevice * pPhysicalDevice() const = 0;

		virtual IQueueHandle selectQueue(QueueType queueType) = 0;
	};



	struct IOnDevice {
		virtual ~IOnDevice();
		virtual IDevice * pDevice() const = 0;
	};

}



#endif // ERHI_DEVICE_HPP