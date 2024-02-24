#pragma once

#include <memory>

namespace erhi {

	template <typename T>
	std::shared_ptr<T> to_shared(T * && ptr) {
		std::shared_ptr<T> result;
		try {
			result.reset(ptr);
		}
		catch (std::bad_alloc const &) {
			delete ptr;
			throw;
		}
		return result;
	}

	template <typename T>
	std::unique_ptr<T> to_unique(T * && ptr) {
		return std::unique_ptr<T>(ptr);
	}

}