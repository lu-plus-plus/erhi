#pragma once

#include "common.hpp"

#include "magic_enum.hpp"
#include "magic_enum_format.hpp"



template <>
struct magic_enum::customize::enum_range<erhi::BufferUsageFlagBits> {
	static constexpr bool is_flags = true;
};

template <>
struct magic_enum::customize::enum_range<erhi::TextureUsageFlagBits> {
	static constexpr bool is_flags = true;
};