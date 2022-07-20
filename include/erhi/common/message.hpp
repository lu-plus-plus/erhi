#ifndef ERHI_MESSAGE_HPP
#define ERHI_MESSAGE_HPP

#include "object.hpp"



namespace erhi {

	enum class MessageType {
		General,
		Validation,
		Performance
	};

	enum class MessageSeverity {
		Error,
		Warning,
		Info,
		Verbose
	};

	struct IMessageCallback : IObject {

	public:

		virtual void operator()(MessageType type, MessageSeverity severity) const = 0;
		
		virtual ~IMessageCallback() = 0;

	};

	using MessageCallbackHandle = Handle<IMessageCallback>;

	struct DefaultMessageCallback : IMessageCallback {

		virtual void operator()(MessageType type, MessageSeverity severity) const override;

		virtual ~DefaultMessageCallback() override;

	};

}



#endif // ERHI_MESSAGE_HPP