#include "erhi/common/message.hpp"
#include "erhi/common/instance.hpp"



namespace erhi {

	IInstance::IInstance(InstanceDesc const & desc) : IObject{}, mpMessageCallback{ desc.pMessageCallback } {}

	IInstance::~IInstance() = default;

}