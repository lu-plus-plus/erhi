#include <format>
#include <iostream>

#include "default_message_callback.hpp"



#define ERHI_LOG_FORMAT_BEGIN "\033["
#define ERHI_LOG_FORMAT_CONCAT ";"
#define ERHI_LOG_FORMAT_END "m"

#define ERHI_LOG_FORE_RED ERHI_LOG_FORMAT_BEGIN "31" ERHI_LOG_FORMAT_END
#define ERHI_LOG_FORE_GREEN ERHI_LOG_FORMAT_BEGIN "32" ERHI_LOG_FORMAT_END
#define ERHI_LOG_FORE_YELLOW ERHI_LOG_FORMAT_BEGIN "33" ERHI_LOG_FORMAT_END

#define ERHI_LOG_RESET ERHI_LOG_FORMAT_BEGIN "0" ERHI_LOG_FORMAT_END

#define ERHI_LOG_PREFIX_ERROR ERHI_LOG_FORE_RED "[error]" ERHI_LOG_RESET
#define ERHI_LOG_PREFIX_WARNING ERHI_LOG_FORE_YELLOW "[warning]" ERHI_LOG_RESET
#define ERHI_LOG_PREFIX_INFO ERHI_LOG_FORE_GREEN "[info]" ERHI_LOG_RESET
#define ERHI_LOG_PREFIX_VERBOSE ERHI_LOG_FORE_GREEN "[verbose]" ERHI_LOG_RESET
#define ERHI_LOG_PREFIX_UNKNOWN ERHI_LOG_FORE_GREEN "[unknown]" ERHI_LOG_RESET



namespace erhi {

	DefaultMessageCallback::DefaultMessageCallback(MessageSeverity minSeverity) : mMinSeverity(minSeverity) {}

	DefaultMessageCallback::~DefaultMessageCallback() = default;

	void DefaultMessageCallback::operator()(MessageType type, MessageSeverity severity, char const * pMessage) const {
		if (severity < mMinSeverity) return;

		switch (severity) {
			case MessageSeverity::Error: {
				std::cout << std::format("{} {}", ERHI_LOG_PREFIX_ERROR, pMessage) << std::endl;
			} break;

			case MessageSeverity::Warning: {
				std::cout << std::format("{} {}", ERHI_LOG_PREFIX_WARNING, pMessage) << std::endl;
			} break;

			case MessageSeverity::Info: {
				std::cout << std::format("{} {}", ERHI_LOG_PREFIX_INFO, pMessage) << std::endl;
			} break;

			case MessageSeverity::Verbose: {
				std::cout << std::format("{} {}", ERHI_LOG_PREFIX_VERBOSE, pMessage) << std::endl;
			} break;

			default: {
				std::cout << std::format("{} {}", ERHI_LOG_PREFIX_UNKNOWN, pMessage) << std::endl;
			} break;
		}

		return;
	}

}