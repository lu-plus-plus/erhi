#pragma once

#include <cstdint>

#include "handle.hpp"



namespace erhi {

	using Flags = int32_t;



	// module: context

	// message
	
	enum class MessageType {
		General,
		Debug,
		Performance
	};

	enum class MessageSeverity {
		Verbose,
		Info,
		Warning,
		Error
	};

	// instance, physical device, and device

	enum class PhysicalDevicePreference {
		HighPerformance,
		MinimalPower
	};

	struct DeviceDesc {
		bool enableDebug;
		PhysicalDevicePreference physicalDevicePreference;
	};



	// module: command

	// queue

	enum class QueueType {
		Primary,
		AsyncCompute,
		AsyncCopy
	};



	// module: resource

	// memory

	enum class MemoryHeapType {
		Default		= 0,
		Upload		= 1,
		ReadBack	= 2,
		/* <todo> Direct </todo> */
	};

	// buffer

	enum BufferUsageFlagBits {
		BufferUsageCopySource = 0x0000'0001,
		BufferUsageCopyTarget = 0x0000'0002,
		BufferUsageShaderResource = 0x0000'0004,
		BufferUsageUnorderedAccess = 0x0000'0008,
		BufferUsageIndexBuffer = 0x0000'0010,
		BufferUsageVertexBuffer = 0x0000'0020
	};

	using BufferUsageFlags = Flags;

	struct BufferDesc {
		BufferUsageFlags usage;
		uint64_t size;
	};

	// texture

	enum TextureUsageFlagBits {
		TextureUsageCopySource = 0x0000'0001,
		TextureUsageCopyTarget = 0x0000'0002,

		/* It seems that there exists no concept like read-only texture or readable-and-writable texture in D3D12. */
		TextureUsageShaderResource = 0x0000'0004,
		TextureUsageUnorderedAccess = 0x0000'0008,

		/* It seems that there exists no concepts like "Sampled" and "Storage" in D3D12 at all. */
		TextureUsageSampling = 0x0000'0010,
		TextureUsageLoadStoreAtomic = 0x0000'0020,

		TextureUsageRenderTarget = 0x0000'0040,
		TextureUsageDepthStencil = 0x0000'0080,
		/*
			<todo> Are these following flags really needed on PC platform? </todo>
			TextureUsageTransientAttachment = 0x0000'0040,
			TextureUsageInputAttachment = 0x0000'0080,
		*/
	};

	using TextureUsageFlags = Flags;

	#define ConcatChannels(r, g, b, a, type) r ## g ## b ## a ## _ ## type
	#define Channel4(r, g, b, a, bits, type) ConcatChannels(r ## bits, g ## bits, b ## bits, a ## bits, type)
	#define Channel3(r, g, b, bits, type) ConcatChannels(r ## bits, g ## bits, b ## bits, , type)
	#define Channel2(r, g, bits, type) ConcatChannels(r ## bits, g ## bits, , , type)
	#define Channel1(r, bits, type) ConcatChannels(r ## bits, , , , type)

	enum class Format {
		Unknown,

		Channel4(R, G, B, A, 32, Typeless),
		Channel4(R, G, B, A, 32, Float),
		Channel4(R, G, B, A, 32, UInt),
		Channel4(R, G, B, A, 32, SInt),

		Channel3(R, G, B, 32, Typeless),
		Channel3(R, G, B, 32, Float),
		Channel3(R, G, B, 32, UInt),
		Channel3(R, G, B, 32, SInt),

		Channel4(R, G, B, A, 16, Typeless),
		Channel4(R, G, B, A, 16, Float),
		Channel4(R, G, B, A, 16, UNorm),
		Channel4(R, G, B, A, 16, UInt),
		Channel4(R, G, B, A, 16, SNorm),
		Channel4(R, G, B, A, 16, SInt),

		Channel2(R, G, 32, Typeless),
		Channel2(R, G, 32, Float),
		Channel2(R, G, 32, UInt),
		Channel2(R, G, 32, SInt),

		Channel4(R, G, B, A, 8, Typeless),
		Channel4(R, G, B, A, 8, UNorm),
		Channel4(R, G, B, A, 8, UInt),
		Channel4(R, G, B, A, 8, SNorm),
		Channel4(R, G, B, A, 8, SInt),

		Channel2(R, G, 16, Typeless),
		Channel2(R, G, 16, Float),
		Channel2(R, G, 16, UNorm),
		Channel2(R, G, 16, UInt),
		Channel2(R, G, 16, SNorm),
		Channel2(R, G, 16, SInt),

		Channel1(R, 32, Float),
		Channel1(R, 32, UInt),
		Channel1(R, 32, SInt),

		D32_Float,
		D16_UNorm,
		D24_UNorm_S8_UInt
	};

	#undef Channel1
	#undef Channel2
	#undef Channel3
	#undef Channel4
	#undef ConcatChannels

	enum class TextureDimension {
		Texture1D,
		Texture2D,
		Texture3D
	};

	enum class TextureTiling {
		Optimal,
		Linear
	};

	enum class TextureSampleCount {
		Count_1,
		Count_2,
		Count_4,
		Count_8,
		Count_16,
		Count_32,
		Count_64
	};

	enum class TextureLayout {
		Undefined,
		Common,
		Present,
		ShaderResource,
		UnorderedAccess,
		RenderTarget,
		DepthStencilWrite,
		DepthStencilRead,
		CopySource,
		CopyTarget,
	};

	struct TextureDesc {
		TextureDimension dimension;
		uint32_t extent[3];
		Format format;
		uint32_t mipLevels;
		TextureSampleCount sampleCount;
		TextureUsageFlags usage;
		TextureTiling tiling;
		TextureLayout initialLayout;
		QueueType initialQueueType;
	};

	struct BufferShaderResourceViewDesc {
		Format format;
		uint32_t strideInBytes;
		uint64_t offsetInItems;
		uint64_t countInItems;
	};

	enum class TextureShaderResourceViewDimension {
		// ... ...
	};

	struct TextureShaderResourceViewDesc {
		Format format;
		// ... ...
	};

}