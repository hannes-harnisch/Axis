#include "PCH.hpp"

#include "Axis/Core/Reporter.hpp"
#include "Axis/Core/Translate.hpp"
#include "Launch.hpp"

namespace ax
{
	ExitCode compile(Span<std::string_view> args)
	{
		return translate(args[0]);
	}

	ExitCode launch(Span<std::string_view> args, Reporter&)
	{
		if(args.empty())
		{
			std::printf("No parameters specified.");
			return ExitCode::Usage;
		}
		return compile(args);
	}
}

#if AX_BUILD_EXE
int main(int argc, char* argv[])
{
	std::vector<std::string_view> args(argv + 1, argv + argc);

	ax::Reporter reporter;
	return std::to_underlying(ax::launch(args, reporter));
}
#endif
