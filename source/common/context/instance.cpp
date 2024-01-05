#include "erhi/common/context/context.hpp"



namespace erhi {

	IInstance::IInstance(InstanceDesc const & desc) : mEnableDebug(desc.enableDebug), mpMessageCallback(desc.pMessageCallback) {}

	IInstance::~IInstance() = default;

}