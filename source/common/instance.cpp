#include "erhi/common/instance.hpp"
#include "erhi/common/message.hpp"



namespace erhi {

	IInstance::IInstance(InstanceDesc const & desc) : IObject{}, mpMessageCallback{ desc.pMessageCallback } {}

	IInstance::~IInstance() = default;

}