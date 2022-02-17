#pragma once

#include <magic_enum/include/magic_enum.hpp>

#include <algorithm>
#include <type_traits>

namespace ax
{
	using magic_enum::enum_cast;
	using magic_enum::enum_count;
	using magic_enum::enum_name;

	template<typename T>
	requires std::is_enum_v<T>
	constexpr size_t size_from_enum_max()
	{
		auto values = magic_enum::enum_values<T>();
		auto max	= std::max_element(values.begin(), values.end());
		return static_cast<size_t>(*max) + 1;
	}
}
