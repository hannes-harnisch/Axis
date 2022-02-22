#pragma once

#include <cstdint>

namespace ax
{
	// Should categorize any possible message the compiler might report.
	enum class Message
	{
		IllegalChar,
		UnterminatedString,
		NameTooLong,
		LiteralTooLong,
		EscapedNonKeyword
	};

	consteval char const* get_format(Message msg)
	{
		using enum Message;
		switch(msg)
		{
			case IllegalChar: return "illegal character '{}'";
			case UnterminatedString: return "unterminated string";
			case NameTooLong: return "name is too long";
			case LiteralTooLong: return "literal is too long, embed it as an asset instead";
			case EscapedNonKeyword: return "{} is not a keyword that can be escaped";
		}
	}

	// Determines whether the number of parameters in the parameter pack matches the number of placeholders in the format string
	// associated with the message.
	template<typename... Ts> consteval bool check_message(Message msg)
	{
		size_t count = 0;

		auto format = get_format(msg);
		while(*format)
			if(*format++ == '{')
				if(*format++ == '}')
					++count;

		return count == sizeof...(Ts);
	}
}
