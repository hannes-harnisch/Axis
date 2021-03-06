#include "PCH.hpp"

#include "Axis/Util/UniqueFile.hpp"
#include "Source.hpp"

namespace ax
{
	std::optional<std::string> get_file_content(std::string_view path_text)
	{
		std::string path(path_text);

		UniqueFile file;
		if(fopen_s(std::out_ptr(file), path.data(), "rb"))
			return {};

		std::fseek(file.get(), 0, SEEK_END);
		long size = std::ftell(file.get());
		std::fseek(file.get(), 0, SEEK_SET);

		std::string content(size, '\0');
		std::fread(content.data(), size, 1, file.get());

		return content;
	}

	std::optional<Source> Source::from(std::string_view path)
	{
		auto source_text = get_file_content(path);

		if(source_text)
			return Source(std::move(*source_text), path);

		return {};
	}

	Source::Iterator Source::begin() const
	{
		return text.begin();
	}

	Source::Iterator Source::end() const
	{
		return text.end();
	}

	SourceLocation Source::locate(Iterator cursor) const
	{
		auto locator = text.begin();

		unsigned line = 1, column = 1;
		while(locator != cursor)
		{
			++column;
			if(*locator++ == '\n')
			{
				++line;
				column = 1;
			}
		}
		return {path, line, column};
	}

	std::string_view Source::get_text() const
	{
		return text;
	}

	std::string_view Source::get_path() const
	{
		return path;
	}

	Source::Source(std::string text, std::string_view path) : text(std::move(text)), path(path)
	{}
}
