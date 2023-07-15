#ifndef ERHI_INSTANCE_HPP
#define ERHI_INSTANCE_HPP

#include <vector>

#include "../common.hpp"



namespace erhi {

	struct IInstance : IObject {

		bool const						mIsDebugEnabled;
		IMessageCallbackHandle const	mMessageCallbackHandle;

		IInstance(InstanceDesc const & desc);
		virtual ~IInstance();

		virtual std::vector<IPhysicalDeviceHandle> ListPhysicalDevices() = 0;

		virtual IPhysicalDeviceHandle SelectPhysicalDevice(PhysicalDeviceDesc const & desc) = 0;

	};



	namespace vk {

		IInstanceHandle CreateInstance(InstanceDesc const & desc);
	
	}

	namespace dx12 {

		IInstanceHandle CreateInstance(InstanceDesc const & desc);

	}

}



#endif // ERHI_INSTANCE_HPP