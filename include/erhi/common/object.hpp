#ifndef ERHI_OBJECT_HPP
#define ERHI_OBJECT_HPP

#include <cstdint>
#include <atomic>



namespace erhi {

	struct [[nodiscard]] IObject {

	private:

		std::atomic<uint32_t> mCount;

	protected:

		IObject() : mCount(0u) {}

		IObject(IObject const &) = delete;
		IObject operator=(IObject const &) = delete;

		IObject(IObject && other) = delete;
		IObject operator=(IObject &&) = delete;

		virtual ~IObject() = default;

	public:

		uint32_t incRef() { return ++mCount; }

		uint32_t release() {
			uint32_t const rest{ --mCount };
			if (rest == 0u) {
				delete this;
			}
			return rest;
		}

	};

}



#endif // ERHI_OBJECT_HPP