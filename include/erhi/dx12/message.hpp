#pragma once

#include "../common/message.hpp"
#include "native.hpp"

namespace erhi::dx12 {

	void AdaptToMessageCallback(
		D3D12_MESSAGE_CATEGORY category,
		D3D12_MESSAGE_SEVERITY severity,
		D3D12_MESSAGE_ID id,
		LPCSTR pDescription,
		void * pContext
	);

}