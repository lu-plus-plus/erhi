#ifndef ERHI_DX12_INSTANCE_HPP
#define ERHI_DX12_INSTANCE_HPP

#include "../common/instance.hpp"



namespace erhi::dx12 {

	struct Instance : IInstance {

		Instance(InstanceDesc const& desc);

		~Instance();

		virtual std::vector<IPhysicalDeviceHandle> listPhysicalDevices() override;

		virtual IPhysicalDeviceHandle selectPhysicalDevice(PhysicalDeviceDesc const& desc) override;

	};

}



#endif // ERHI_DX12_INSTANCE_HPP