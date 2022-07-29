#ifndef ERHI_EXAMPLES_UTILS_DEFAULT_MESSAGE_CALLBACK_HPP
#define ERHI_EXAMPLES_UTILS_DEFAULT_MESSAGE_CALLBACK_HPP

#include "erhi/common/message.hpp"



namespace erhi {

	struct DefaultMessageCallback : IMessageCallback {

		MessageSeverity const mMinSeverity;

		DefaultMessageCallback(MessageSeverity minSeverity);

		virtual ~DefaultMessageCallback() override;

		virtual void operator()(MessageType type, MessageSeverity severity, char const * pMessage) const override;

	};

}



#endif // ERHI_EXAMPLES_UTILS_DEFAULT_MESSAGE_CALLBACK_HPP