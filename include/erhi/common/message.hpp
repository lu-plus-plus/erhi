#ifndef ERHI_MESSAGE_HPP
#define ERHI_MESSAGE_HPP

#include <string>

#include "handle.hpp"
#include "object.hpp"



namespace erhi {

	enum class MessageType : uint32_t {
		General,
		Validation,
		Performance
	};

	enum class MessageSeverity : uint32_t {
		Verbose,
		Info,
		Warning,
		Error
	};



	struct IMessageCallback : IObject {

		virtual void operator()(MessageType type, MessageSeverity severity, char const * pMessage) const = 0;
		
		void verbose(char const * pMessage);
		void info(char const * pMessage);
		void warning(char const * pMessage);
		void error(char const * pMessage);

		void verbose(std::string const & message);
		void info(std::string const & message);
		void warning(std::string const & message);
		void error(std::string const & message);

		virtual ~IMessageCallback();

	};

}



#endif // ERHI_MESSAGE_HPP