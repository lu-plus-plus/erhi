#pragma once

#include <ostream>

#include "../context/message.hpp"



namespace erhi {

	struct StreamMessageCallback : IMessageCallback {
		std::ostream & mOS;
		MessageSeverity const mMinSeverity;

		StreamMessageCallback(std::ostream & os, MessageSeverity minSeverity);

		virtual ~StreamMessageCallback() override;

		virtual void operator()(MessageType type, MessageSeverity severity, char const * pMessage) const override;
	};

}