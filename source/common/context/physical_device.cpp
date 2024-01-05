#include "erhi/common/context/context.hpp"



namespace erhi {

	IPhysicalDevice::IPhysicalDevice(PhysicalDeviceDesc const & desc) : mDesc(desc) {}
	IPhysicalDevice::~IPhysicalDevice() = default;

}