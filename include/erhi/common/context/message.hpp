#ifndef ERHI_MESSAGE_HPP
#define ERHI_MESSAGE_HPP

#include <string>

#include "../common.hpp"



namespace erhi {

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