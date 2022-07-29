#include "spdlog/spdlog.h"

#include "default_message_callback.hpp"



namespace erhi {

	DefaultMessageCallback::DefaultMessageCallback(MessageSeverity minSeverity) : mMinSeverity(minSeverity) {}

	DefaultMessageCallback::~DefaultMessageCallback() = default;

	void DefaultMessageCallback::operator()(MessageType type, MessageSeverity severity, char const * pMessage) const {
		if (severity < mMinSeverity) return;

		switch (severity) {
			case MessageSeverity::Error: {
				spdlog::error("{}", pMessage);
			} break;

			case MessageSeverity::Warning: {
				spdlog::warn("{}", pMessage);
			} break;

			case MessageSeverity::Info: {
				spdlog::info("{}", pMessage);
			} break;

			case MessageSeverity::Verbose: {
				spdlog::set_pattern("[%Y-%m-%d %T.%e] [%^verbose%$] %v");
				spdlog::info("{}", pMessage);
				spdlog::set_pattern("%+");
			} break;

			default: {
				spdlog::warn("message callback: unknown message severity");
				spdlog::warn("{}", pMessage);
			} break;
		}

		return;
	}

}