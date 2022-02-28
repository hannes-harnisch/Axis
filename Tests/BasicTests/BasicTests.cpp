#include "Axis/Core/Launch.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest/doctest.h>

#define AX_PATH(path) std::string_view("../../../Tests/"##path)

using namespace ax;

Reporter run_default(std::string_view source_path)
{
	Reporter reporter(stderr);
	CHECK(launch(source_path, reporter) == ExitCode::Ok);
	return reporter;
}

TEST_CASE("SimpleMain")
{
	run_default(AX_PATH("BasicTests/SimpleMain.ax"));
}

TEST_CASE("IllegalChar")
{
	auto args	  = AX_PATH("BasicTests/IllegalChar.ax");
	auto reporter = run_default(args);
	CHECK(reporter.has_message(Message::IllegalChar, {args, 4, 3}, int('`')));
}

TEST_CASE("UnterminatedString")
{
	auto args	  = AX_PATH("BasicTests/UnterminatedString.ax");
	auto reporter = run_default(args);
	CHECK(reporter.has_message(Message::UnterminatedString, {args, 3, 11}, {}));
}

TEST_CASE("NameTooLong")
{
	auto args	  = AX_PATH("BasicTests/NameTooLong.ax");
	auto reporter = run_default(args);
	CHECK(reporter.has_message(Message::NameTooLong, {args, 1, 12}, {}));
}

TEST_CASE("LiteralTooLong")
{
	auto args	  = AX_PATH("BasicTests/LiteralTooLong.ax");
	auto reporter = run_default(args);
	CHECK(reporter.has_message(Message::LiteralTooLong, {args, 3, 9}, {}));
}

TEST_CASE("CommentTooLong")
{
	auto args	  = AX_PATH("BasicTests/CommentTooLong.ax");
	auto reporter = run_default(args);
	CHECK(reporter.has_message(Message::CommentTooLong, {args, 1, 1}, {}));
}

TEST_CASE("EscapedNonKeyword")
{
	auto args	  = AX_PATH("BasicTests/EscapedNonKeyword.ax");
	auto reporter = run_default(args);
	CHECK(reporter.has_message(Message::EscapedNonKeyword, {args, 1, 1}, {"foo"}));
}
