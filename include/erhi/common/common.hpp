#ifndef ERHI_COMMON_HPP
#define ERHI_COMMON_HPP

#include "object.hpp"
#include "handle.hpp"

namespace erhi {

	// message

	enum class MessageType : uint32_t {
		General,
		Validation,
		Performance
	};

	enum class MessageSeverity : uint32_t {
		Verbose,
		Info,
		Warning,
		Error
	};

	// instance

	struct InstanceDesc {
		bool					enableDebug;
		IMessageCallbackHandle	pMessageCallback;
	};

	// physical device

	enum class PhysicalDeviceType : uint32_t {
		Integrated, Discrete
	};

	struct PhysicalDeviceDesc {
		PhysicalDeviceType		type;
	};

	// device

	struct DeviceDesc {
	};

	// queue

	enum class QueueType : uint32_t {
		Graphics,
		Compute,
		Copy
	};

}

#endif // ERHI_COMMON_HPP