#pragma once

#include "../common.hpp"



namespace erhi {

	struct IInstance {
		bool mEnableDebug;
		std::shared_ptr<IMessageCallback> mpMessageCallback;

		IInstance(InstanceDesc const & desc);

		virtual IPhysicalDeviceHandle SelectPhysicalDevice(PhysicalDeviceDesc const & desc) = 0;
	};

	namespace vk {
		IInstanceHandle CreateInstance(InstanceDesc const & desc);
	}

	namespace dx12 {
		IInstanceHandle CreateInstance(InstanceDesc const & desc);
	}

}