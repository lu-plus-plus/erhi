#ifndef ERHI_VULKAN_INSTANCE_HPP
#define ERHI_VULKAN_INSTANCE_HPP

#include "../common/instance.hpp"
#include "native.hpp"



namespace erhi::vk {

	struct Instance : IInstance {

		static std::atomic<bool>			gIsVolkInitialized;

		VkInstance							mInstance;
		std::vector<VkPhysicalDevice>		mPhysicalDevices;

		Instance(InstanceDesc const & desc);

		~Instance();
		
		virtual std::vector<IPhysicalDeviceHandle> listPhysicalDevices() const override;
		
		virtual IPhysicalDeviceHandle selectDefaultPhysicalDevice() const override;

	};

}



#endif // ERHI_VULKAN_INSTANCE_HPP