#include "PCH.hpp"

#include "Axis/Lex/Lexer.hpp"
#include "Translate.hpp"

#include <iostream> // TODO::remove

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
		return content += '\n';
	}

	ExitCode translate(std::string_view path, Reporter& reporter)
	{
		auto file_content = get_file_content(path);
		if(!file_content)
			return ExitCode::NoInput;

		auto tokens = lex(*file_content, reporter);
		std::cout << tokens.to_string(*file_content);
		return ExitCode::Ok;
	}
}
