#include "PCH.hpp"

#include "Axis/Lex/Lexer.hpp"
#include "Translate.hpp"

namespace ax
{
	std::optional<std::string> get_file_content(std::string_view path_text)
	{
		std::string path(path_text);

		FILE* file;
		auto  error = fopen_s(&file, path.data(), "r");
		if(error)
			return {};

		std::fseek(file, 0, SEEK_END);
		long size = std::ftell(file);
		std::fseek(file, 0, SEEK_SET);

		std::string content;
		content.resize(size);
		std::fread(content.data(), size, 1, file);

		std::fclose(file);
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
