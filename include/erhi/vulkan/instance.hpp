#ifndef ERHI_VULKAN_INSTANCE_HPP
#define ERHI_VULKAN_INSTANCE_HPP

#include "../common/instance.hpp"
#include "native.hpp"



namespace erhi::vk {

	struct Instance : IInstance {

		static std::atomic<bool>			gIsVolkInitialized;

		VkInstance							mInstance;
		VkDebugUtilsMessengerEXT			mDebugUtilsMessenger;
		std::vector<VkPhysicalDevice>		mPhysicalDevices;

		Instance(InstanceDesc const & desc);

		~Instance();
		
		virtual std::vector<IPhysicalDeviceHandle> listPhysicalDevices() override;
		
		virtual IPhysicalDeviceHandle selectPhysicalDevice(PhysicalDeviceDesc const & desc) override;

	};

}



#endif // ERHI_VULKAN_INSTANCE_HPP