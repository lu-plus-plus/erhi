#ifndef ERHI_PHYSICAL_DEVICE_HPP
#define ERHI_PHYSICAL_DEVICE_HPP

#include <cstdint>

#include "handle.hpp"
#include "object.hpp"



namespace erhi {

	enum class PhysicalDeviceType : uint32_t {
		Integrated, Discrete
	};

	struct IPhysicalDevice : IObject {

		IPhysicalDevice();

		virtual ~IPhysicalDevice();

		virtual char const * name() const = 0;
		virtual PhysicalDeviceType type() const = 0;

		virtual DeviceHandle createDevice() const = 0;

	};

}



#endif // ERHI_PHYSICAL_DEVICE_HPP