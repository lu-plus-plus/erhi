#include "erhi/common/message.hpp"



namespace erhi {

	void IMessageCallback::verbose(char const * pMessage) {
		(*this)(MessageType::General, MessageSeverity::Verbose, pMessage);
	}

	void IMessageCallback::info(char const * pMessage) {
		(*this)(MessageType::General, MessageSeverity::Info, pMessage);
	}

	void IMessageCallback::warning(char const * pMessage) {
		(*this)(MessageType::General, MessageSeverity::Warning, pMessage);
	}

	void IMessageCallback::error(char const * pMessage) {
		(*this)(MessageType::General, MessageSeverity::Error, pMessage);
	}

	void IMessageCallback::verbose(std::string const & message) {
		verbose(message.c_str());
	}

	void IMessageCallback::info(std::string const & message) {
		info(message.c_str());
	}

	void IMessageCallback::warning(std::string const & message) {
		warning(message.c_str());
	}

	void IMessageCallback::error(std::string const & message) {
		error(message.c_str());
	}

	IMessageCallback::~IMessageCallback() = default;

}