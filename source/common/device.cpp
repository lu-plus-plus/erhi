#include "erhi/common/device.hpp"
#include "erhi/common/physical_device.hpp"



namespace erhi {

	IDevice::IDevice(DeviceDesc const &) : IObject{} {}

	IDevice::~IDevice() = default;

}