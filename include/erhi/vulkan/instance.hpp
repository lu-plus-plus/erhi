#ifndef ERHI_VULKAN_INSTANCE_HPP
#define ERHI_VULKAN_INSTANCE_HPP

#include "../common/instance.hpp"
#include "../common/message.hpp"
#include "native.hpp"
#include "physical_device.hpp"



namespace erhi::vk {

	struct Instance : IInstance {

		static std::atomic<bool>			gIsVolkInitialized;

		VkInstance							mInstance;
		std::vector<PhysicalDeviceHandle>	mpPhysicalDevices;

		Instance(InstanceDesc const & desc);

		~Instance();
		
		virtual std::vector<PhysicalDeviceHandle> listPhysicalDevices() const override;
		
	};

}



#endif // ERHI_VULKAN_INSTANCE_HPP