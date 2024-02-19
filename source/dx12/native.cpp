#include <comdef.h>

#include <print>

#include "erhi/dx12/native.hpp"



namespace erhi::dx12 {

	char const * ErrorCode(HRESULT result) {
		return windows::ErrorCode(result);
	}

	void ThrowOnError(HRESULT result, char const * statement, std::source_location const location) {
		windows::ThrowOnError(result, statement, location);
	}

	void ExitOnError(HRESULT result, char const * statement, std::source_location const location) noexcept {
		windows::ExitOnError(result, statement, location);
	}

}



namespace erhi::dx12::mapping {

	D3D12_HEAP_TYPE MapMemoryHeapType(MemoryHeapType heapType) {
		switch (heapType) {
			case erhi::MemoryHeapType::Default:
				return D3D12_HEAP_TYPE_DEFAULT;
				break;
			case erhi::MemoryHeapType::Upload:
				return D3D12_HEAP_TYPE_UPLOAD;
				break;
			case erhi::MemoryHeapType::ReadBack:
			default:
				return D3D12_HEAP_TYPE_READBACK;
				break;
		}
	}

	DXGI_FORMAT MapFormat(Format format) {
		switch (format) {
			case Format::Unknown:
				return DXGI_FORMAT_UNKNOWN;

			case Format::R32G32B32A32_Typeless:
				return DXGI_FORMAT_R32G32B32A32_TYPELESS;
			case Format::R32G32B32A32_Float:
				return DXGI_FORMAT_R32G32B32A32_FLOAT;
			case Format::R32G32B32A32_UInt:
				return DXGI_FORMAT_R32G32B32A32_UINT;
			case Format::R32G32B32A32_SInt:
				return DXGI_FORMAT_R32G32B32A32_SINT;

			case Format::R32G32B32_Typeless:
				return DXGI_FORMAT_R32G32B32_TYPELESS;
			case Format::R32G32B32_Float:
				return DXGI_FORMAT_R32G32B32_FLOAT;
			case Format::R32G32B32_UInt:
				return DXGI_FORMAT_R32G32B32_UINT;
			case Format::R32G32B32_SInt:
				return DXGI_FORMAT_R32G32B32_SINT;

			case Format::R16G16B16A16_Typeless:
				return DXGI_FORMAT_R16G16B16A16_TYPELESS;
			case Format::R16G16B16A16_Float:
				return DXGI_FORMAT_R16G16B16A16_FLOAT;
			case Format::R16G16B16A16_UNorm:
				return DXGI_FORMAT_R16G16B16A16_UNORM;
			case Format::R16G16B16A16_UInt:
				return DXGI_FORMAT_R16G16B16A16_UINT;
			case Format::R16G16B16A16_SNorm:
				return DXGI_FORMAT_R16G16B16A16_SNORM;
			case Format::R16G16B16A16_SInt:
				return DXGI_FORMAT_R16G16B16A16_SINT;

			case Format::R32G32_Typeless:
				return DXGI_FORMAT_R32G32_TYPELESS;
			case Format::R32G32_Float:
				return DXGI_FORMAT_R32G32_FLOAT;
			case Format::R32G32_UInt:
				return DXGI_FORMAT_R32G32_UINT;
			case Format::R32G32_SInt:
				return DXGI_FORMAT_R32G32_SINT;

			case Format::R8G8B8A8_Typeless:
				return DXGI_FORMAT_R8G8B8A8_TYPELESS;
			case Format::R8G8B8A8_UNorm:
				return DXGI_FORMAT_R8G8B8A8_UNORM;
			case Format::R8G8B8A8_UInt:
				return DXGI_FORMAT_R8G8B8A8_UINT;
			case Format::R8G8B8A8_SNorm:
				return DXGI_FORMAT_R8G8B8A8_SNORM;
			case Format::R8G8B8A8_SInt:
				return DXGI_FORMAT_R8G8B8A8_SINT;
			
			case Format::B8G8R8A8_UNorm:
				return DXGI_FORMAT_B8G8R8A8_UNORM;
			case Format::B8G8R8A8_UNormSRGB:
				return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

			case Format::R16G16_Typeless:
				return DXGI_FORMAT_R16G16_TYPELESS;
			case Format::R16G16_Float:
				return DXGI_FORMAT_R16G16_FLOAT;
			case Format::R16G16_UNorm:
				return DXGI_FORMAT_R16G16_UNORM;
			case Format::R16G16_UInt:
				return DXGI_FORMAT_R16G16_UINT;
			case Format::R16G16_SNorm:
				return DXGI_FORMAT_R16G16_SNORM;
			case Format::R16G16_SInt:
				return DXGI_FORMAT_R16G16_SINT;

			case Format::R32_Float:
				return DXGI_FORMAT_R32_FLOAT;
			case Format::R32_UInt:
				return DXGI_FORMAT_R32_UINT;
			case Format::R32_SInt:
				return DXGI_FORMAT_R32_SINT;

			case Format::D32_Float:
				return DXGI_FORMAT_D32_FLOAT;
			case Format::D16_UNorm:
				return DXGI_FORMAT_D16_UNORM;
			case Format::D24_UNorm_S8_UInt:
				return DXGI_FORMAT_D24_UNORM_S8_UINT;
		}

		return DXGI_FORMAT_UNKNOWN;
	}

}