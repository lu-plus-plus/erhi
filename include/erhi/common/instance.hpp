#ifndef ERHI_INSTANCE_HPP
#define ERHI_INSTANCE_HPP

#include <vector>

#include "common.hpp"



namespace erhi {

	struct IInstance : IObject {

		IMessageCallbackHandle const mMessageCallbackHandle;

		IInstance(InstanceDesc const & desc);
		virtual ~IInstance();

		virtual std::vector<IPhysicalDeviceHandle> listPhysicalDevices() = 0;

		virtual IPhysicalDeviceHandle selectPhysicalDevice(PhysicalDeviceDesc const & desc) = 0;

	};



	namespace vk {

		IInstanceHandle createInstance(InstanceDesc const & desc);
	
	}

	namespace dx12 {

		IInstanceHandle createInstance(InstanceDesc const & desc);

	}

}



#endif // ERHI_INSTANCE_HPP