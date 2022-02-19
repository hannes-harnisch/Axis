#pragma once

#include "Axis/Core/ExitCode.hpp"
#include "Axis/Core/Reporter.hpp"
#include "Axis/Lex/Token.hpp"

#include <string_view>

namespace ax
{
	ExitCode translate(std::string_view path, Reporter& reporter);
}
