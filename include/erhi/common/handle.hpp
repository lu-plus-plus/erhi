#ifndef ERHI_HANDLE_HPP
#define ERHI_HANDLE_HPP

#include <concepts>
#include <utility>



namespace erhi {
	
	template <typename Derived, typename Base>
	concept strictly_derived_from = std::derived_from<Derived, Base> and not std::same_as<Derived, Base>;



	template <typename T>
	struct [[nodiscard]] Handle {

		template <typename U>
		friend struct Handle;

		template <typename T, typename ... Args>
		requires std::constructible_from<T, Args...>
		friend Handle<T> MakeHandle(Args && ... args);

		template <typename To, typename From>
		friend Handle<To> dynamic_handle_cast(Handle<From> const & from);

		template <typename To, typename From>
		friend Handle<To> dynamic_handle_cast(Handle<From> && from);

	protected:

		T * pRaw;

	public:

		Handle() : pRaw(nullptr) {}

		explicit Handle(nullptr_t) : pRaw(nullptr) {}

		Handle & operator=(nullptr_t) {
			if (pRaw) pRaw->release();

			pRaw = nullptr;

			return *this;
		}

		~Handle() noexcept {
			if (pRaw) pRaw->release();
		}

		// Constructing handle from a raw pointer.

		Handle(T * ptr) : pRaw(ptr) {
			if (pRaw) pRaw->incRef();
		}

		Handle & operator=(T * ptr) = delete;

		// Casting from handles of the same type.

		Handle(Handle const & other) : pRaw(other.pRaw) {
			if (pRaw) pRaw->incRef();
		}

		Handle & operator=(Handle const & other) {
			if (pRaw) pRaw->release();

			pRaw = other.pRaw;
			if (pRaw) pRaw->incRef();
			
			return *this;
		}

		Handle(Handle && other) noexcept : pRaw(other.pRaw) {
			other.pRaw = nullptr;
		}

		Handle & operator=(Handle && other) noexcept {
			if (pRaw) pRaw->release();

			pRaw = other.pRaw;
			other.pRaw = nullptr;

			return *this;
		}

		// Casting from handles of derived types.

		template <strictly_derived_from<T> Derived>
		Handle(Handle<Derived> const & other) : pRaw(static_cast<T *>(other.pRaw)) {}

		template <strictly_derived_from<T> Derived>
		Handle & operator=(Handle<Derived> const & other) {
			if (pRaw) pRaw->release();

			pRaw = static_cast<T *>(other.pRaw);
			if (pRaw) pRaw->incRef();

			return *this;
		}

		template <strictly_derived_from<T> Derived>
		Handle(Handle<Derived> && other) noexcept : pRaw(static_cast<T *>(other.pRaw)) {
			other.pRaw = nullptr;
		}

		template <strictly_derived_from<T> Derived>
		Handle & operator=(Handle<Derived> && other) noexcept {
			if (pRaw) pRaw->release();

			pRaw = static_cast<T *>(other.pRaw);
			other.pRaw = nullptr;

			return *this;
		}

		// pointer-like behavior

		T * operator->() const noexcept {
			return pRaw;
		}

		T & operator*() const noexcept {
			return *pRaw;
		}

		T * get() const noexcept {
			return pRaw;
		}

		operator bool() const noexcept {
			return pRaw != nullptr;
		}

	};



	template <typename T, typename ... Args>
	requires std::constructible_from<T, Args...>
	Handle<T> MakeHandle(Args && ... args) {
		T * pRaw = new T{ std::forward<Args>(args)... };
		
		Handle<T> handle;

		handle.pRaw = pRaw;
		if (pRaw) pRaw->incRef();

		return handle;
	}



	template <typename To, typename From>
	Handle<To> dynamic_handle_cast(Handle<From> const & from) {
		Handle<To> to;
		
		to.pRaw = dynamic_cast<To *>(from.pRaw);
		if (to.pRaw) to.pRaw->incRef();

		return to;
	}

	template <typename To, typename From>
	Handle<To> dynamic_handle_cast(Handle<From> && from) {
		Handle<To> to;

		to.pRaw = dynamic_cast<To *>(from.pRaw);
		from.pRaw = nullptr;

		return to;
	}



	using IMessageCallbackHandle	= Handle<struct IMessageCallback>;
	using IInstanceHandle			= Handle<struct IInstance>;
	using IPhysicalDeviceHandle		= Handle<struct IPhysicalDevice>;
	using IDeviceHandle				= Handle<struct IDevice>;

	using IQueueHandle				= Handle<struct IQueue>;

	using IMemoryHandle				= Handle<struct IMemory>;
	
	using IBufferHandle				= Handle<struct IBuffer>;
	using IPlacedBufferHandle		= Handle<struct IPlacedBuffer>;
	using ICommittedBufferHandle	= Handle<struct ICommittedBuffer>;

	using ITextureHandle			= Handle<struct ITexture>;
	using IPlacedTextureHandle		= Handle<struct IPlacedTexture>;
	using ICommittedTextureHandle	= Handle<struct ICommittedTexture>;



	namespace vk {

		using MessageCallbackHandle		= Handle<struct MessageCallback>;
		using InstanceHandle			= Handle<struct Instance>;
		using PhysicalDeviceHandle		= Handle<struct PhysicalDevice>;
		using DeviceHandle				= Handle<struct Device>;

		using QueueHandle				= Handle<struct Queue>;

		using MemoryHandle			= Handle<struct Memory>;
		using BufferHandle			= Handle<struct Buffer>;
		using TextureHandle			= Handle<struct Texture>;
		// using AllocatorHandle		= Handle<struct Allocator>;

	}
	


	namespace dx12 {

		using MessageCallbackHandle		= Handle<struct MessageCallback>;
		using InstanceHandle			= Handle<struct Instance>;
		using PhysicalDeviceHandle		= Handle<struct PhysicalDevice>;
		using DeviceHandle				= Handle<struct Device>;

		using QueueHandle				= Handle<struct Queue>;

		using MemoryHandle			= Handle<struct Memory>;
		using BufferHandle			= Handle<struct Buffer>;
		using TextureHandle			= Handle<struct Texture>;
		// using AllocatorHandle		= Handle<struct Allocator>;

	}

}



#endif // ERHI_HANDLE_HPP