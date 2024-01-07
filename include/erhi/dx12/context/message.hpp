#pragma once

#include "../../common/context/message.hpp"
#include "../native.hpp"

namespace erhi::dx12 {

	void MapD3D12DebugMessage(
		D3D12_MESSAGE_CATEGORY category,
		D3D12_MESSAGE_SEVERITY severity,
		D3D12_MESSAGE_ID id,
		LPCSTR pDescription,
		void * pContext
	);

}