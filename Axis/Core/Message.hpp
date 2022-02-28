#pragma once

#include <cstdint>
#include <string_view>

namespace ax
{
	// Should categorize any possible message the compiler might report.
	enum class Message
	{
		IllegalChar,
		UnterminatedString,
		NameTooLong,
		LiteralTooLong,
		CommentTooLong,
		EscapedNonKeyword
	};

	constexpr std::string_view get_format(Message msg)
	{
		using enum Message;
		switch(msg)
		{
			case IllegalChar: return "illegal character '{}'";
			case UnterminatedString: return "unterminated string";
			case NameTooLong: return "name is too long";
			case LiteralTooLong: return "literal is too long, embed it as an asset instead";
			case CommentTooLong: return "comment is too long";
			case EscapedNonKeyword: return "{} is not a keyword that can be escaped";
		}
		return "";
	}

	// Determines whether the number of parameters in the parameter pack matches the number of placeholders in the format string
	// associated with the message.
	template<typename... Ts> consteval bool check_message(Message msg)
	{
		size_t count = 0;

		auto format = get_format(msg).data();
		while(*format)
			if(*format++ == '{')
				if(*format++ == '}')
					++count;

		return count == sizeof...(Ts);
	}
}
