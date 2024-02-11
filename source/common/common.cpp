#include "erhi/common/common.hpp"
#include "erhi/common/exception.hpp"

#include "magic_enum.hpp"

#include <format>



namespace erhi {

	constexpr uint32_t to_unsigned_integer(std::string_view const & s) {
		uint32_t number = 0u;

		uint32_t exponent = 1u;
		for (int i = int(s.size()) - 1; i >= 0; --i) {
			number += (s[i] - '0') * exponent;
			exponent *= 10u;
		}

		return number;
	}

	constexpr uint32_t GetFormatSizeInBits(Format format) {
		auto token = magic_enum::enum_name(format);
		
		uint32_t bits = 0;

		for (size_t i = 0; i < token.size(); /* in loop body */) {
			if (std::isdigit(token[i])) {
				size_t ending = i + 1;
				while (ending < token.size() and std::isdigit(token[ending]))
					ending += 1;
				bits += to_unsigned_integer(std::string_view(token.begin() + i, token.begin() + ending));

				i = ending;
			}
			else {
				i += 1;
			}
		}

		return bits;
	}

	GlobalConstants::GlobalConstants() : FormatSizeInBytes{} {
		for (size_t i = 0; i < static_cast<size_t>(Format::EnumCount); ++i) {
			uint32_t const bits = GetFormatSizeInBits(static_cast<Format>(i));
			if (bits % 8 != 0) {
				throw std::runtime_error(std::format("unexpected bit count {} returned from erhi::Format {} in initializing global constants", bits, i));
			}
			FormatSizeInBytes[i] = bits / 8;
		}
	}

}