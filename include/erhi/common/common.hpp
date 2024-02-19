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

	enum class CommandListLifetime {
		ShortLived,
		Persistent
	};

	enum class CommandListLevel {
		Direct,
		Bundle
	};

	enum CommandListUsageFlags {
		CommandListUsageOneTime = 0x01
	};

	struct CommandPoolDesc {
		QueueType queueType;
		CommandListLifetime lifetime;
	};

	struct CommandListDesc {
		CommandListLevel level;
	};

	struct CommandListBeginInfo {
		CommandListUsageFlags usageFlags;
	};

	struct VertexBufferView {
		IBuffer * pBuffer;
		uint64_t offset;
		uint64_t size;
		uint64_t stride;
	};

	enum class PrimitiveTopology {
		PointList = 0,
		LineList = 1,
		LineStrip = 2,
		TriangleList = 3,
		TriangleStrip = 4
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
		BufferUsageConstantBuffer = 0x0000'0010,
		BufferUsageIndexBuffer = 0x0000'0020,
		BufferUsageVertexBuffer = 0x0000'0040,
		BufferUsageShaderAtomic = 0x0000'0080
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

		/* There is no concept like read-only texture or readable-and-writable texture in Vulkan,
		 * while there is no fine-grained control like Sampled Image (sampling) and Storage Image (load, store, and atomic operations) in D3D12.
		 * At here, we keep the abstraction as fine-grained as possible.
		 *
		 * TextureUsageShaderResource = 0x0000'0004,
		   TextureUsageUnorderedAccess = 0x0000'0008,
		 */

		/*
		 * <todo> Are these following flags needed in a render pass with only one subpass? </todo>
		 *
		 * TextureUsageTransientAttachment = 0x0000'0040,
		 * TextureUsageInputAttachment = 0x0000'0080,
		 */

		/* It seems that there exists no concepts like "Sampled" and "Storage" in D3D12 at all.
		 * A SRV of a texture may be sampled, or have a specific texel fetched.
		 * An UAV of a texture cannot be sampled, but may load from or store to a texel.
		 *
		 * We are corresponding sampled image to SRV, and storage image to UAV.
		 *
		 * TextureUsageSampling = TextureUsageShaderResource,
		 * TextureUsageStorage = TextureUsageUnorderedAccess,
		 * TextureUsageLoadStoreAtomic = TextureUsageUnorderedAccess,
		 */
		TextureUsageShaderResource = 0x0000'0004,
		TextureUsageUnorderedAccess = 0x0000'0008,

		TextureUsageRenderTarget = 0x0000'0010,
		TextureUsageDepthStencil = 0x0000'0020,
		TextureUsageShaderAtomic = 0x0000'0040
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

		Channel4(B, G, R, A, 8, UNorm),
		Channel4(B, G, R, A, 8, UNormSRGB),

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
		D24_UNorm_S8_UInt,

		EnumCount
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

	enum BufferDescriptorFlagBits {
		BufferDescriptorAllowByteAddressBuffer = 0x0001
	};

	using BufferDescriptorFlags = Flags;

	struct BufferDescriptorDesc {
		Format format;
		uint32_t structureSizeInBytes;
		uint64_t offsetInElements;
		uint64_t countInElements;
		BufferDescriptorFlags flagBits;
	};

	enum class TextureViewDimension {
		Texture1D,
		Texture2D,
		Texture3D
	};

	enum TextureAspectFlagBits {
		TextureAspectColor = 0x0001,
		TextureAspectDepth = 0x0002,
		TextureAspectStencil = 0x0004
	};

	using TextureAspectFlags = Flags;

	struct TextureViewDesc {
		TextureViewDimension dimension;
		Format format;
		uint32_t mostDetailedMipLevel;
		uint32_t mipLevelCount;
		TextureAspectFlagBits aspectFlags;
	};

	enum class DescriptorHeapType {
		CBV_SRV_UAV,
		Sampler
	};

	struct DescriptorHeapDesc {
		uint64_t sizeInBytes;
		DescriptorHeapType type;
	};

	enum class DescriptorType {
		Sampler,
		BufferShaderResource,
		BufferUnorderedAccess,
		BufferConstantBuffer,
		TextureShaderResource,
		TextureUnorderedAccess
	};

	enum ShaderStageFlagBits {
		ShaderStageVertex = 0x0001,
		ShaderStagePixel = 0x0002,
		ShaderStageCompute = 0x0004,
		ShaderStageAllGraphics = ShaderStageVertex | ShaderStagePixel,
		ShaderStageAll = 0x7FFF'FFFF,
	};

	using ShaderStageFlags = Flags;

	struct DescriptorSetLayoutBinding {
		DescriptorType descriptorType;
		uint32_t descriptorCount;
		uint32_t registerSpace;
		uint32_t firstRegister;
		ShaderStageFlags shaderStageFlags;
	};

	struct DescriptorSetLayoutDesc {
		uint32_t bindingCount;
		DescriptorSetLayoutBinding const * bindings;
		DescriptorHeapType descriptorHeapType;
	};

	enum class PipelineBindPoint {
		Graphics,
		Compute
	};

	enum class AttachmentLoadOp {
		Load,
		Clear,
		DoNotCare
	};

	enum class AttachmentStoreOp {
		Store,
		DoNotCare
	};

	struct AttachmentDesc {
		Format format;
		TextureSampleCount sampleCount;
		AttachmentLoadOp loadOp;
		AttachmentStoreOp storeOp;
		AttachmentLoadOp stencilLoadOp;
		AttachmentStoreOp stencilStoreOp;
		TextureLayout initialLayout;
		TextureLayout subpassLayout;
		TextureLayout finalLayout;
	};

	struct RenderPassDesc {
		PipelineBindPoint pipelineBindPoint;
		uint32_t renderTargetAttachmentCount;
		AttachmentDesc const * renderTargetAttachments;
		AttachmentDesc const * resolveAttachments;
		AttachmentDesc const * pDepthStencilAttachment;
	};

	struct FrameBufferDesc {
		IRenderPassHandle pRenderPass;
		uint32_t attachmentCount;
		ITextureView const * attachments;
		uint32_t width;
		uint32_t height;
	};



	// module: present

	struct IWindowMessageCallback {
		virtual void OnRender();
	};

	struct WindowDesc {
		uint32_t width;
		uint32_t height;
		uint32_t left;
		uint32_t top;
		char const * windowName;
		IWindowMessageCallback pMessageCallback;
	};

	struct SwapChainDesc {
		IWindowHandle pWindow;
		Format format;
		uint32_t bufferCount;
		TextureUsageFlags usageFlags;
	};

}



namespace erhi {

	struct GlobalConstants {
		GlobalConstants();
		uint32_t FormatSizeInBytes[static_cast<size_t>(Format::EnumCount)];
	};

}