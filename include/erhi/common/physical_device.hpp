#ifndef ERHI_PHYSICAL_DEVICE_HPP
#define ERHI_PHYSICAL_DEVICE_HPP

#include <cstdint>

#include "object.hpp"



namespace erhi {

	enum class PhysicalDeviceType : uint32_t {
		Integrated, Discrete
	};

	struct IPhysicalDevice : IObject {

		IPhysicalDevice();

		virtual ~IPhysicalDevice();

		virtual uint32_t deviceID() const = 0;
		virtual char const * deviceName() const = 0;
		virtual PhysicalDeviceType deviceType() const = 0;

	};

	using PhysicalDeviceHandle = Handle<IPhysicalDevice>;

}



#endif // ERHI_PHYSICAL_DEVICE_HPP