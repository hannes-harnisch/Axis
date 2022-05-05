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
	auto reporter = run_default(AX_PATH("LexTests/SimpleMain.ax"));
	CHECK(reporter.has_no_messages());
}

TEST_CASE("IllegalChar")
{
	auto args	  = AX_PATH("LexTests/IllegalChar.ax");
	auto reporter = run_default(args);
	CHECK(reporter.pop_message(Message::IllegalChar, {args, 4, 3}, 7));
	CHECK(reporter.has_no_messages());
}

TEST_CASE("NameTooLong")
{
	auto args	  = AX_PATH("LexTests/NameTooLong.ax");
	auto reporter = run_default(args);
	CHECK(reporter.pop_message(Message::TokenTooLong, {args, 1, 12}, {}));
	CHECK(reporter.has_no_messages());
}

TEST_CASE("StringTooLong")
{
	auto args	  = AX_PATH("LexTests/LiteralTooLong.ax");
	auto reporter = run_default(args);
	CHECK(reporter.pop_message(Message::TokenTooLong, {args, 3, 9}, {}));
	CHECK(reporter.has_no_messages());
}

TEST_CASE("CommentTooLong")
{
	auto args	  = AX_PATH("LexTests/CommentTooLong.ax");
	auto reporter = run_default(args);
	CHECK(reporter.pop_message(Message::TokenTooLong, {args, 1, 1}, {}));
	CHECK(reporter.has_no_messages());
}

TEST_CASE("MissingClosingQuote")
{
	auto args	  = AX_PATH("LexTests/MissingClosingQuote.ax");
	auto reporter = run_default(args);
	CHECK(reporter.pop_message(Message::MissingClosingQuote, {args, 3, 24}, {}));
	CHECK(reporter.pop_message(Message::MissingClosingQuote, {args, 4, 13}, {}));
	CHECK(reporter.has_no_messages());
}

TEST_CASE("EscapedNonKeyword")
{
	auto args	  = AX_PATH("LexTests/EscapedNonKeyword.ax");
	auto reporter = run_default(args);
	CHECK(reporter.pop_message(Message::EscapedNonKeyword, {args, 1, 2}, {"foo"}));
	CHECK(reporter.has_no_messages());
}
