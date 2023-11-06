#pragma once

#include "../common.hpp"

#include <memory>



namespace erhi {

	struct IMemory : IObject {
		MemoryRequirements mRequirements;

		IMemory(MemoryRequirements const & requirements);
		virtual ~IMemory() = 0;

		virtual IDeviceHandle GetDevice() const = 0;
	};
	


	struct IBuffer : IObject {
		BufferDesc mDesc;

		IBuffer(BufferDesc const & desc);
		virtual ~IBuffer() = 0;
	};



	struct ITexture : IObject {
		TextureDesc mDesc;

		ITexture(TextureDesc const & desc);
		virtual ~ITexture() = 0;
	};

}


