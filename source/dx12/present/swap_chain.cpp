#include "erhi/dx12/context/context.hpp"
#include "erhi/dx12/present/present.hpp"
#include "erhi/dx12/command/command.hpp"

namespace erhi::dx12 {

	SwapChain::SwapChain(DeviceHandle pDevice, WindowHandle pWindow, SwapChainDesc const & desc) : ISwapChain(desc), mpDevice(pDevice), mpWindow(pWindow) {
		DXGI_SWAP_CHAIN_DESC1 const swapChainDesc = {
			.Width = 0,
			.Height = 0,
			.Format = mapping::MapFormat(desc.format),
			.Stereo = false,
			.SampleDesc = DXGI_SAMPLE_DESC{
				.Count = 1,
				.Quality = 0
			},
			.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount = desc.bufferCount,
			.Scaling = DXGI_SCALING_NONE,
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,
			.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
			.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
		};

		ComPtr<IDXGISwapChain1> pSwapChain;

		D3D12CheckResult(mpDevice->mpDXGIFactory->CreateSwapChainForHwnd(
			mpDevice->mpPrimaryQueue->mpCommandQueue.Get(),
			mpWindow->mWindowHandle,
			&swapChainDesc,
			nullptr,
			nullptr,
			pSwapChain.GetAddressOf()
		));

		D3D12CheckResult(pSwapChain->QueryInterface(IID_PPV_ARGS(mpSwapChainLatest.GetAddressOf())));

		mSwapChainTextures.resize(desc.bufferCount);

		for (uint32_t i = 0; i < desc.bufferCount; ++i) {
			ComPtr<ID3D12Resource> pResource;
			pSwapChain->GetBuffer(i, IID_PPV_ARGS(pResource.GetAddressOf()));
			
			auto const d3d12Desc = pResource->GetDesc();

			if (d3d12Desc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D) {
				throw std::runtime_error("swap chain buffers' dimension is not 2D texture");
			}

			TextureDesc const textureDesc = {
				.dimension = TextureDimension::Texture2D,
				.extent = { UINT(d3d12Desc.Width), d3d12Desc.Height, 1 },
				.format = desc.format,
				.mipLevels = 1,
				.sampleCount = TextureSampleCount::Count_1,
				.usage = desc.usageFlags,
				.tiling = TextureTiling::Linear,
				.initialLayout = TextureLayout::Undefined,
				.initialQueueType = QueueType::Primary
			};

			mSwapChainTextures[i] = new Texture(pResource, textureDesc);
		}
	}
	
	SwapChain::~SwapChain() = default;

	ITextureHandle SwapChain::GetTexture(uint32_t index) {
		return mSwapChainTextures[index];
	}

}