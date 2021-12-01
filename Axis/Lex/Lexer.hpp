#pragma once

#include "Axis/Lex/Token.hpp"

#include <string>
#include <vector>

namespace ax
{
	std::vector<Token> lex(std::string const& file_content);
}
