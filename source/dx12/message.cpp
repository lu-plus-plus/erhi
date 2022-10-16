#include "erhi/dx12/message.hpp"

namespace erhi::dx12 {

	static MessageType MapMessageType(D3D12_MESSAGE_CATEGORY) {
		return MessageType::Validation;
	}

	static MessageSeverity MapMessageSeverity(D3D12_MESSAGE_SEVERITY severity) {
		switch (severity) {
			case D3D12_MESSAGE_SEVERITY_CORRUPTION:
			case D3D12_MESSAGE_SEVERITY_ERROR: {
				return MessageSeverity::Error;
			}
			break;

			case D3D12_MESSAGE_SEVERITY_WARNING: {
				return MessageSeverity::Warning;
			}
			break;

			case D3D12_MESSAGE_SEVERITY_INFO: {
				return MessageSeverity::Info;
			}
			break;

			case D3D12_MESSAGE_SEVERITY_MESSAGE: {
				return MessageSeverity::Verbose;
			}
			break;
			
			default: {
				return MessageSeverity::Verbose;
			}
			break;
		}
	}

	void AdaptToMessageCallback(
		D3D12_MESSAGE_CATEGORY category,
		D3D12_MESSAGE_SEVERITY severity,
		D3D12_MESSAGE_ID id,
		LPCSTR pDescription,
		void * pContext) {

		IMessageCallback const & callback{ *static_cast<IMessageCallback const *>(pContext) };

		callback(MapMessageType(category), MapMessageSeverity(severity), pDescription);
	}

}