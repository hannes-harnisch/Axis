#include "Axis/Core/Launch.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest/doctest.h>

#define AX_TEST_PATH "../../../Tests/"

TEST_CASE("SimpleMain")
{
	ax::Reporter	 reporter;
	std::string_view args = AX_TEST_PATH "BasicTests/SimpleMain.ax";
	REQUIRE(ax::launch(args, reporter) == ax::ExitCode::Ok);
}
