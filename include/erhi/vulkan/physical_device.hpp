#ifndef ERHI_VULKAN_PHYSICAL_DEVICE_HPP
#define ERHI_VULKAN_PHYSICAL_DEVICE_HPP

#include <vector>

#include "../common/physical_device.hpp"
#include "native.hpp"



namespace erhi::vk {

	struct PhysicalDevice : IPhysicalDevice {
		
		InstanceHandle							mInstanceHandle;

		VkPhysicalDevice						mPhysicalDevice;
		VkPhysicalDeviceProperties2				mProperties;
		VkPhysicalDeviceFeatures2				mFeatures;
		VkPhysicalDeviceMemoryProperties2		mMemoryProperties;
		std::vector<VkExtensionProperties>		mExtensions;
		std::vector<VkQueueFamilyProperties2>	mQueueFamilies;

		PhysicalDevice(Instance * pInstance, VkPhysicalDevice physicalDevice);
		~PhysicalDevice();

		operator VkPhysicalDevice() const;

		virtual IInstance *				pInstance() const override;
		virtual char const *			name() const override;
		virtual PhysicalDeviceType		type() const override;
		virtual bool					isCacheCoherentUMA() const override;
		virtual IDeviceHandle			createDevice(DeviceDesc const & desc) override;

	};

}



#endif // ERHI_VULKAN_PHYSICAL_DEVICE_HPP