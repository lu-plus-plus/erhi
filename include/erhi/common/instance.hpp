#ifndef ERHI_INSTANCE_HPP
#define ERHI_INSTANCE_HPP

#include <vector>

#include "handle.hpp"
#include "object.hpp"



namespace erhi {

	struct InstanceDesc {
		bool					enableDebug;
		IMessageCallbackHandle	pMessageCallback;
	};



	struct IInstance : IObject {

		IMessageCallbackHandle const mpMessageCallback;

		IInstance(InstanceDesc const & desc);
		
		virtual ~IInstance();

		virtual std::vector<IPhysicalDeviceHandle> listPhysicalDevices() = 0;

		virtual IPhysicalDeviceHandle selectDefaultPhysicalDevice() = 0;

	};



	namespace vk {

		IInstanceHandle createInstance(InstanceDesc const & desc);
	
	}

}



#endif // ERHI_INSTANCE_HPP