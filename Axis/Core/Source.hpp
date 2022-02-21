#pragma once

#include "Axis/Core/Reporter.hpp"

#include <optional>
#include <string>
#include <string_view>

namespace ax
{
	class Source
	{
	public:
		using Iterator = std::string::const_iterator;

		static std::optional<Source> from(std::string_view path);

		Iterator		 begin() const;
		Iterator		 end() const;
		SourceLocation	 locate(Iterator cursor) const;
		std::string_view get_path() const;

	private:
		std::string		 text;
		std::string_view path;

		Source(std::string text, std::string_view path);
	};
}
