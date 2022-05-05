#pragma once

#include "Axis/Util/Enum.hpp"

namespace ax
{
	template<typename Key, typename Value, usize COUNT = size_from_enum_max<Key>()>
	class LookupTable
	{
	public:
		constexpr Value& operator[](Key key)
		{
			return table[static_cast<usize>(key)];
		}

		constexpr Value operator[](Key key) const
		{
			return table[static_cast<usize>(key)];
		}

	private:
		Value table[COUNT] = {};
	};
}
