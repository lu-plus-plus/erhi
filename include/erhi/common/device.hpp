#ifndef ERHI_COMMON_DEVICE_HPP
#define ERHI_COMMON_DEVICE_HPP

#include <memory>

#include "object.hpp"
#include "message.hpp"



namespace erhi {

	enum class DevicePhysicalType : uint32_t {
		integrated, discrete
	};

	struct DeviceDesc {
		DevicePhysicalType devicePhysicalType;
		std::unique_ptr<MessageCallback> debugMessageCallback;
	};

	class IDevice : IObject {

		IDevice(DeviceDesc && desc);

		virtual ~IDevice() = 0;

	};

	using DeviceHandle = Handle<IDevice>;

}



#endif // ERHI_COMMON_DEVICE_HPP