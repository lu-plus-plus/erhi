#ifndef ERHI_HANDLE_HPP
#define ERHI_HANDLE_HPP

#include <concepts>
#include <utility>



namespace erhi {
	
	template <typename T>
	struct [[nodiscard]] Handle {

	private:

		T * pRaw;

	public:

		Handle() : pRaw(nullptr) {}

		Handle(nullptr_t) : pRaw(nullptr) {}

		template <typename Derived>
		friend struct Handle;

		template <std::derived_from<T> Derived>
		Handle(Derived * ptr) : pRaw(ptr) {
			if (pRaw) pRaw->incRef();
		}

		template <std::derived_from<T> Derived>
		Handle(Handle<Derived> const & pDerived) : pRaw(pDerived.pRaw) {
			if (pRaw) pRaw->incRef();
		}

		Handle(Handle const & other) : pRaw(other.pRaw) {
			if (pRaw) pRaw->incRef();
		}

		Handle & operator=(Handle const & other) {
			if (pRaw) pRaw->release();

			pRaw = other.pRaw;
			if (pRaw) pRaw->incRef();

			return *this;
		}

		Handle(Handle && other) : pRaw(other.pRaw) {
			other.pRaw = nullptr;
		}

		Handle & operator=(Handle && other) {
			if (pRaw) pRaw->release();

			pRaw = other.pRaw;
			other.pRaw = nullptr;

			return *this;
		}

		~Handle() {
			if (pRaw) pRaw->release();
		}

		// pointer-like behavior

		T * operator->() const {
			return pRaw;
		}

		T & operator*() const {
			return *pRaw;
		}

		T * get() const {
			return pRaw;
		}

		operator bool() const {
			return pRaw != nullptr;
		}

	};



	template <typename T, typename ... Args>
	requires std::constructible_from<T, Args...>
	Handle<T> MakeHandle(Args && ... args) {
		T * pRaw = new T{ std::forward<Args>(args)... };
		return Handle<T>{ pRaw };
	}



	template <typename To, typename From>
	Handle<To> dynamic_handle_cast(Handle<From> const & from) {
		return Handle<To>{ dynamic_cast<To *>(from.get()) };
	}



	using IMessageCallbackHandle	= Handle<struct IMessageCallback>;
	using IInstanceHandle			= Handle<struct IInstance>;
	using IPhysicalDeviceHandle		= Handle<struct IPhysicalDevice>;
	using IDeviceHandle				= Handle<struct IDevice>;

	using IQueueHandle				= Handle<struct IQueue>;



	namespace vk {

		using MessageCallbackHandle = Handle<struct MessageCallback>;
		using InstanceHandle = Handle<struct Instance>;
		using PhysicalDeviceHandle = Handle<struct PhysicalDevice>;
		using DeviceHandle = Handle<struct Device>;

		using QueueHandle = Handle<struct Queue>;

	}
	


	namespace dx12 {

		using MessageCallbackHandle = Handle<struct MessageCallback>;
		using InstanceHandle = Handle<struct Instance>;
		using PhysicalDeviceHandle = Handle<struct PhysicalDevice>;
		using DeviceHandle = Handle<struct Device>;

		using QueueHandle = Handle<struct Queue>;

	}

}



#endif // ERHI_HANDLE_HPP