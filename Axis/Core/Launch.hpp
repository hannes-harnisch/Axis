#pragma once

#include "Axis/Core/ExitCode.hpp"
#include "Axis/Core/Reporter.hpp"
#include "Axis/Util/Span.hpp"

#include <span>
#include <string_view>

namespace ax
{
	ExitCode launch(Span<std::string_view> args, Reporter& reporter);
}
