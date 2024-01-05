#pragma once

#include "../common.hpp"



namespace erhi {

	struct IPhysicalDevice {
		PhysicalDeviceDesc mDesc;

		IPhysicalDevice(PhysicalDeviceDesc const & desc);
		virtual ~IPhysicalDevice();

		virtual IInstanceHandle		GetInstance() const = 0;
		virtual IDeviceHandle		CreateDevice(DeviceDesc const & desc) = 0;
	};

}