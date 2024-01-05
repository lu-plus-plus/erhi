#pragma once

#include <cstdint>

#include "handle.hpp"



namespace erhi {

	using Flags = int32_t;



	// module: context

	// message
	
	enum class MessageType {
		General,
		Validation,
		Performance
	};

	enum class MessageSeverity {
		Verbose,
		Info,
		Warning,
		Error
	};

	// instance

	struct InstanceDesc {
		bool enableDebug;
		IMessageCallback * pMessageCallback;
	};

	// physical device

	enum class PhysicalDevicePreference {
		HighPerformance,
		MinimalPower
	};

	struct PhysicalDeviceDesc {
		PhysicalDevicePreference preference;
	};

	// device

	struct DeviceDesc {
		/* nothing for now */
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
		BufferUsageUniformBuffer = 0x0000'0004,
		BufferUsageStorageBuffer = 0x0000'0008,
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
		/*
			<todo>
			It seems that the "Sampled" flag is not needed in D3D12 at all, where samplers and textures are separate objects.
			The problem is, is it possible to separate them in Vulkan, instead of binding them together as combined image sampler?
			</todo>
		*/
		TextureUsageSampled = 0x0000'0004,
		TextureUsageStorage = 0x0000'0008,
		TextureUsageRenderTargetAttachment = 0x0000'0010,
		TextureUsageDepthStencilAttachment = 0x0000'0020,
		/*
			<todo>
			Are these following flags really needed on PC platform?
			</todo>
		*/
		TextureUsageTransientAttachment = 0x0000'0040,
		TextureUsageInputAttachment = 0x0000'0080,
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

	struct TextureDesc {
		TextureDimension dimension;
		uint32_t extent[3];
		Format format;
		uint32_t mipLevels;
		TextureSampleCount sampleCount;
		TextureUsageFlags usage;
		TextureTiling tiling;
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