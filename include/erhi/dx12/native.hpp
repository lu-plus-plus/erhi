#pragma once

#ifndef NOMINMAX
	#define NOMINMAX
#endif

#include <wrl/client.h>			// ComPtr
#include <dxgi1_6.h>			// DXGI
#include <d3d12.h>				// D3D12
#include "D3D12MemAlloc.h"		// D3D12 Memory Allocator

#include "../common/handle.hpp"			// forward declaration for abstract interfaces
#include "../common/common.hpp"			// descriptive structs and enums
#include "../common/exception.hpp"		// exception
#include "../common/h_result.hpp"		// handling HRESULT



namespace erhi::dx12 {
	using Microsoft::WRL::ComPtr;
	
	using IDXGIFactoryLatest = IDXGIFactory7;
	using IDXGIAdapterLatest = IDXGIAdapter4;

	using ID3D12DebugLatest = ID3D12Debug6;
	using ID3D12DeviceLatest = ID3D12Device10;
	using ID3D12InfoQueueLatest = ID3D12InfoQueue1;

	using ID3D12GraphicsCommandListLatest = ID3D12GraphicsCommandList7;

	char const * ErrorCode(HRESULT result);

	void ThrowOnError(HRESULT result, char const * statement, std::source_location const location = std::source_location::current());
	
	void ExitOnError(HRESULT result, char const * statement, std::source_location const location = std::source_location::current()) noexcept;
}

#define D3D12CheckResult(stat) if (HRESULT result = (stat); result != S_OK) erhi::dx12::ThrowOnError(result, #stat, std::source_location::current())

#define D3D12ExitOnError(stat) if (HRESULT result = (stat); result != S_OK) erhi::dx12::ExitOnError(result, #stat, std::source_location::current())
