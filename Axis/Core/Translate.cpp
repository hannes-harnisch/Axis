#include "PCH.hpp"

#include "Axis/Core/Source.hpp"
#include "Axis/Lex/Lexer.hpp"
#include "Translate.hpp"

namespace ax
{
	ExitCode translate(std::string_view path, Reporter& reporter)
	{
		auto source = Source::from(path);
		if(!source)
			return ExitCode::NoInput;

		auto tokens = lex(*source, reporter);
		return ExitCode::Ok;
	}
}
