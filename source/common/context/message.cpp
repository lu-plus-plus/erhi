#include "erhi/common/context/context.hpp"



namespace erhi {

	IMessageCallback::IMessageCallback() = default;

	IMessageCallback::~IMessageCallback() = default;

	void IMessageCallback::Verbose(std::string_view message) {
		(*this)(MessageType::General, MessageSeverity::Verbose, message.data());
	}

	void IMessageCallback::Info(std::string_view message) {
		(*this)(MessageType::General, MessageSeverity::Info, message.data());
	}

	void IMessageCallback::Warning(std::string_view message) {
		(*this)(MessageType::General, MessageSeverity::Warning, message.data());
	}

	void IMessageCallback::Error(std::string_view message) {
		(*this)(MessageType::General, MessageSeverity::Error, message.data());
	}

}