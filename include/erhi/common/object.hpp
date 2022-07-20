#ifndef ERHI_OBJECT_HPP
#define ERHI_OBJECT_HPP

#include <cstdint>
#include <atomic>



namespace erhi {

	struct IObject {

	private:

		std::atomic<uint32_t> mCount;

	protected:

		IObject() : mCount(0u) {}

		IObject(IObject const &) = delete;
		IObject operator=(IObject const &) = delete;

		IObject(IObject && other) = delete;
		IObject operator=(IObject &&) = delete;

		virtual ~IObject() = 0;

	public:

		uint32_t incRef() { return ++mCount; }

		uint32_t release() {
			if (--mCount == 0u) {
				delete this;
			}
		}

	};



	template <typename T>
	struct Handle {

	private:

		T * pRaw;

	public:

		Handle() : pRaw(nullptr) {}
		
		Handle(nullptr_t) : pRaw(nullptr) {}

		Handle(T * ptr) : pRaw(ptr) {
			pRaw->incRef();
		}

		Handle(Handle const & other) : pRaw(other.pRaw) {
			pRaw->incRef();
		}

		Handle & operator=(Handle const & other) {
			pRaw->release();

			pRaw = other.pRaw;
			pRaw->incRef();
		}

		Handle(Handle && other) : pRaw(other.pRaw) {
			other.pRaw = nullptr;
		}

		Handle & operator=(Handle && other) {
			pRaw->release();

			pRaw = other.pRaw;
			other.pRaw = nullptr;
		}

		~Handle() {
			if (pRaw) pRaw->release();
		}

	};

	using ObjectHandle = Handle<IObject>;

}



#endif // ERHI_OBJECT_HPP