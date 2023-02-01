#ifndef ERHI_OBJECT_HPP
#define ERHI_OBJECT_HPP

#include <cstdint>



namespace erhi {

	struct [[nodiscard]] IObject {

	private:

		uint32_t mRefCount;

	protected:

		IObject() : mRefCount(0u) {}

		IObject(IObject const &) = delete;
		IObject operator=(IObject const &) = delete;

		IObject(IObject && other) = delete;
		IObject operator=(IObject &&) = delete;

		virtual ~IObject() = default;

	public:

		uint32_t incRef() { return ++mRefCount; }

		uint32_t release() {
			uint32_t const rest = --mRefCount;
			if (rest == 0u) {
				delete this;
			}
			return rest;
		}

	};

}



#endif // ERHI_OBJECT_HPP