#ifndef ERHI_VULKAN_INSTANCE_HPP
#define ERHI_VULKAN_INSTANCE_HPP

#include <atomic>

#include "../../common/context/instance.hpp"
#include "../native.hpp"



namespace erhi::vk {

	struct Instance : IInstance {

		static std::atomic<bool>			gIsVolkInitialized;

		VkInstance							mInstance;
		VkDebugUtilsMessengerEXT			mDebugUtilsMessenger;
		std::vector<VkPhysicalDevice>		mPhysicalDevices;

		Instance(InstanceDesc const & desc);
		~Instance();

		operator VkInstance() const;
		
		virtual std::vector<IPhysicalDeviceHandle>	ListPhysicalDevices() override;
		virtual IPhysicalDeviceHandle				SelectPhysicalDevice(PhysicalDeviceDesc const & desc) override;
		
	};

}



#endif // ERHI_VULKAN_INSTANCE_HPP