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
		Default,
		Upload,
		ReadBack,
		/* <todo> Direct </todo> */
	};

	// buffer

	namespace BufferUsageBits {
		enum : int32_t {
			CopySource = 0x0000'0001,
			CopyTarget = 0x0000'0002,
			UniformTexel = 0x0000'0004,
			StorageTexel = 0x0000'0008,
			Uniform = 0x0000'0010,
			Storage = 0x0000'0020,
			Index = 0x0000'0040,
			Vertex = 0x0000'0080,
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
			ColorAttachment = 0x0000'0010,
			DepthStencilAttachment = 0x0000'0020,
			TransientAttachment = 0x0000'0040,
			InputAttachment = 0x0000'0080,
		};
	}

	using TextureUsageFlags = Flags;

}

#endif // ERHI_COMMON_HPP