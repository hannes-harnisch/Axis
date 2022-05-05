#pragma once

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <limits>
#include <memory>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace stdf = std::filesystem;

using u8	= std::uint8_t;
using u16	= std::uint16_t;
using u32	= std::uint32_t;
using u64	= std::uint64_t;
using usize = std::size_t;

using i8	= std::int8_t;
using i16	= std::int16_t;
using i32	= std::int32_t;
using i64	= std::int64_t;
using isize = std::ptrdiff_t;
