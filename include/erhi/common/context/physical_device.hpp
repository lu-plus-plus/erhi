#ifndef ERHI_PHYSICAL_DEVICE_HPP
#define ERHI_PHYSICAL_DEVICE_HPP

#include "../common.hpp"



namespace erhi {

	struct IPhysicalDevice : IObject {

		IPhysicalDevice();
		virtual ~IPhysicalDevice();

		virtual IInstance *			pInstance() const = 0;
		virtual char const *		name() const = 0;
		virtual PhysicalDeviceType	type() const = 0;
		virtual bool				isCacheCoherentUMA() const = 0;
		virtual IDeviceHandle		createDevice(DeviceDesc const & desc) = 0;

	};

}



#endif // ERHI_PHYSICAL_DEVICE_HPP