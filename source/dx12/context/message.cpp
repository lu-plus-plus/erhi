#include "erhi/dx12/context/message.hpp"

namespace erhi::dx12 {

	namespace {
		MessageSeverity MapMessageSeverity(D3D12_MESSAGE_SEVERITY severity) {
			switch (severity) {
				case D3D12_MESSAGE_SEVERITY_CORRUPTION:
				case D3D12_MESSAGE_SEVERITY_ERROR: {
					return MessageSeverity::Error;
				} break;

				case D3D12_MESSAGE_SEVERITY_WARNING: {
					return MessageSeverity::Warning;
				} break;

				case D3D12_MESSAGE_SEVERITY_INFO: {
					return MessageSeverity::Info;
				} break;

				case D3D12_MESSAGE_SEVERITY_MESSAGE: {
					return MessageSeverity::Verbose;
				} break;

				default: {
					return MessageSeverity::Warning;
				} break;
			}
		}
	}

	void MapD3D12DebugMessage(
		D3D12_MESSAGE_CATEGORY category,
		D3D12_MESSAGE_SEVERITY severity,
		D3D12_MESSAGE_ID id,
		LPCSTR pDescription,
		void * pContext) {

		IMessageCallback & callback{ *static_cast<IMessageCallback *>(pContext) };

		callback(MessageType::Debug, MapMessageSeverity(severity), pDescription);
	}

}