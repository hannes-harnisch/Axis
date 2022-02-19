#pragma once

#include "Axis/Core/Reporter.hpp"
#include "Axis/Lex/TokenStream.hpp"

#include <string>

namespace ax
{
	TokenStream lex(std::string const& file_content, Reporter& reporter);
}
