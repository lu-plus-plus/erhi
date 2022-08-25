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



#define DeclareInterfaceHandle(type) using I ## type ## Handle = Handle<struct I ## type>;

	DeclareInterfaceHandle(Object);

	DeclareInterfaceHandle(Instance);
	DeclareInterfaceHandle(PhysicalDevice);
	DeclareInterfaceHandle(Device);
	DeclareInterfaceHandle(MessageCallback);

	DeclareInterfaceHandle(Queue);

#undef DeclareInterfaceHandle

}



#endif // ERHI_HANDLE_HPP