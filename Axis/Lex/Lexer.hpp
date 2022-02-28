#pragma once

#include "Axis/Core/Reporter.hpp"
#include "Axis/Core/Source.hpp"
#include "Axis/Lex/TokenStream.hpp"

namespace ax
{
	TokenStream lex(Source const& source, Reporter& reporter);
}
