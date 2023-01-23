#ifndef ERHI_COMMON_HPP
#define ERHI_COMMON_HPP

#include "object.hpp"
#include "handle.hpp"

namespace erhi {

	using Flags = int32_t;

	inline constexpr Flags NullFlags = Flags(0);



	// module: context

	// message

	enum class MessageType : uint32_t {
		General,
		Validation,
		Performance
	};

	enum class MessageSeverity : uint32_t {
		Verbose,
		Info,
		Warning,
		Error
	};

	// instance

	struct InstanceDesc {
		bool					enableDebug;
		IMessageCallbackHandle	pMessageCallback;
	};

	// physical device

	enum class PhysicalDeviceType : uint32_t {
		Integrated, Discrete
	};

	struct PhysicalDeviceDesc {
		PhysicalDeviceType		type;
	};

	// device

	struct DeviceDesc {
	};

	// queue

	enum class QueueType : uint32_t {
		Graphics,
		Compute,
		Copy
	};



	// module: resource

	// memory

	enum class MemoryHeapType : uint32_t {
		Default		= 0,
		Upload		= 1,
		ReadBack	= 2,
		MaxEnum		= 3
		/* <todo> Direct </todo> */
	};

	// buffer

	namespace BufferUsageBits {
		enum : int32_t {
			CopySource = 0x0000'0001,
			CopyTarget = 0x0000'0002,
			UniformBuffer = 0x0000'0004,
			StorageBuffer = 0x0000'0008,
			IndexBuffer = 0x0000'0010,
			VertexBuffer = 0x0000'0020
		};
	}

	using BufferUsageFlags = Flags;

	// texture

	namespace TextureUsageBits {
		enum : Flags {
			CopySource = 0x0000'0001,
			CopyTarget = 0x0000'0002,
			Sampled = 0x0000'0004,
			Storage = 0x0000'0008,
			RenderTargetAttachment = 0x0000'0010,
			DepthStencilAttachment = 0x0000'0020,
			TransientAttachment = 0x0000'0040,
			InputAttachment = 0x0000'0080,
		};
	}

	using TextureUsageFlags = Flags;

	struct MemoryRequirements {
		uint32_t memoryTypeIndex;
		uint64_t size;
		uint64_t alignment;
	};

	struct MemoryDesc {
		uint32_t memoryTypeIndex;
		uint64_t size;
	};

	struct BufferDesc {
		BufferUsageFlags bufferUsage;
		uint64_t size;
	};

}

#endif // ERHI_COMMON_HPP