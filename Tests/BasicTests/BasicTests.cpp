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
	CHECK(reporter.has_message(Message::IllegalChar, {args, 4, 3}, 7));
}

TEST_CASE("NameTooLong")
{
	auto args	  = AX_PATH("BasicTests/NameTooLong.ax");
	auto reporter = run_default(args);
	CHECK(reporter.has_message(Message::TokenTooLong, {args, 1, 12}, {}));
}

TEST_CASE("StringTooLong")
{
	auto args	  = AX_PATH("BasicTests/LiteralTooLong.ax");
	auto reporter = run_default(args);
	CHECK(reporter.has_message(Message::StringTooLong, {args, 3, 9}, {}));
}

TEST_CASE("CommentTooLong")
{
	auto args	  = AX_PATH("BasicTests/CommentTooLong.ax");
	auto reporter = run_default(args);
	CHECK(reporter.has_message(Message::TokenTooLong, {args, 1, 1}, {}));
}

TEST_CASE("MissingClosingQuote")
{
	auto args	  = AX_PATH("BasicTests/MissingClosingQuote.ax");
	auto reporter = run_default(args);
	CHECK(reporter.has_message(Message::MissingClosingQuote, {args, 3, 24}, {}));
	CHECK(reporter.has_message(Message::MissingClosingQuote, {args, 4, 13}, {}));
}

TEST_CASE("EscapedNonKeyword")
{
	auto args	  = AX_PATH("BasicTests/EscapedNonKeyword.ax");
	auto reporter = run_default(args);
	CHECK(reporter.has_message(Message::EscapedNonKeyword, {args, 1, 1}, {"foo"}));
}
