#pragma once

#include <string_view>

#include "../common.hpp"



namespace erhi {

	struct IMessageCallback {
		IMessageCallback();
		virtual ~IMessageCallback() = 0;

		virtual void operator()(MessageType type, MessageSeverity severity, char const * pMessage) const = 0;

		void Verbose(std::string_view message);
		void Info(std::string_view message);
		void Warning(std::string_view message);
		void Error(std::string_view message);
	};

}


