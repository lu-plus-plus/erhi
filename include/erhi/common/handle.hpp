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



	#define DeclareHandle(prefix, type) using prefix ## type ## Handle = Handle<struct prefix ## type>

	#define DeclareContextHandles(prefix)			\
		DeclareHandle(prefix, MessageCallback);		\
		DeclareHandle(prefix, Instance);			\
		DeclareHandle(prefix, PhysicalDevice);		\
		DeclareHandle(prefix, Device);				\
		DeclareHandle(prefix, Queue)				\

	#define InterfacePrefix I
	#define VulkanPrefix
	#define DX12Prefix

	DeclareContextHandles(InterfacePrefix);

	namespace vk {
		DeclareContextHandles(VulkanPrefix);
	}
	
	namespace dx12 {
		DeclareContextHandles(DX12Prefix);
	}

	#undef InterfacePrefix
	#undef VulkanPrefix
	#undef DX12Prefix

	#undef DeclareContextHandles

	#undef DeclareHandle

}



#endif // ERHI_HANDLE_HPP