#include "PCH.hpp"

#include "Axis/Lex/Lexer.hpp"
#include "Translate.hpp"

namespace ax
{
	std::optional<std::string> get_file_content(std::string_view path_text)
	{
		std::string path(path_text);

		FILE* file = fopen(path.data(), "r");
		if(!file)
			return {};

		fseek(file, 0, SEEK_END);
		long size = ftell(file);
		fseek(file, 0, SEEK_SET);

		std::string content;
		content.resize(size);
		fread(content.data(), size, 1, file);

		fclose(file);
		return content;
	}

	ExitCode translate(std::string_view path)
	{
		auto file_content = get_file_content(path);
		if(!file_content)
			return ExitCode::NoInput;

		auto tokens = lex(*file_content);
		return ExitCode::Ok;
	}
}
