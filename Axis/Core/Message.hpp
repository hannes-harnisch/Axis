#pragma once

#include <cstdint>

namespace ax
{
	enum class Message
	{
		IllegalChar,
		UnterminatedString,
		StringLiteralTooLong,
	};

	consteval char const* get_format(Message msg)
	{
		using enum Message;
		switch(msg)
		{
			case IllegalChar: return "Illegal character '{}'.";
			case UnterminatedString: return "Unterminated string.";
			case StringLiteralTooLong: return "String literal is too long, embed the string as a text asset instead.";
		}
	}

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
