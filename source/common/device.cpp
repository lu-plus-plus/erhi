#include "erhi/common/device.hpp"
#include "erhi/common/physical_device.hpp"



namespace erhi {

	IDevice::IDevice(IPhysicalDeviceHandle physicalDeviceHandle) : IObject{} {}

	IDevice::~IDevice() = default;

}