#include "erhi/common/context/message.hpp"
#include "erhi/common/context/instance.hpp"



namespace erhi {

	IInstance::IInstance(InstanceDesc const & desc) : IObject{}, mIsDebugEnabled{ desc.enableDebug }, mMessageCallbackHandle { desc.pMessageCallback } {}

	IInstance::~IInstance() = default;

}