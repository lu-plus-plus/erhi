#ifndef ERHI_INSTANCE_HPP
#define ERHI_INSTANCE_HPP

#include <vector>

#include "handle.hpp"
#include "object.hpp"



namespace erhi {

	struct InstanceDesc {
		bool					enableDebug;
		MessageCallbackHandle	pMessageCallback;
	};

	struct IInstance : IObject {
		
		MessageCallbackHandle const mpMessageCallback;

		IInstance(InstanceDesc const & desc);
		
		virtual ~IInstance();

		virtual std::vector<PhysicalDeviceHandle> listPhysicalDevices() const = 0;

		virtual PhysicalDeviceHandle selectDefaultPhysicalDevice() const = 0;

	};



	namespace vk {
		InstanceHandle createInstance(InstanceDesc const & desc);
	}

}



#endif // ERHI_INSTANCE_HPP