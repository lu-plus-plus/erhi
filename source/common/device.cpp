#include "erhi/common/physical_device.hpp"
#include "erhi/common/device.hpp"



namespace erhi {

	IDevice::IDevice(IPhysicalDeviceHandle physicalDeviceHandle) : IObject{} {}

	IDevice::~IDevice() = default;

}