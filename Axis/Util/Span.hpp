#pragma once

#include <span>

namespace ax
{
	template<typename T>
	class Span : public std::span<T const>
	{
		using Base = std::span<T const>;

	public:
		using Base::Base;

		Span(T const&&) = delete;
		Span(T const& object) noexcept : Base(&object, 1)
		{}
	};

	template<typename T>
	class MutSpan : public std::span<T>
	{
		using Base = std::span<T>;

	public:
		using Base::Base;

		MutSpan(T const&&) = delete;
		MutSpan(T& object) noexcept : Base(&object, 1)
		{}
	};
}
