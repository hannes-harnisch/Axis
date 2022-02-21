#include "PCH.hpp"

#include "Char.hpp"

namespace ax
{
	bool is_digit(char ch)
	{
		return ch >= '0' && ch <= '9';
	}

	bool is_whitespace(char ch)
	{
		switch(ch)
		{
			case ' ':
			case '\t':
			case '\r':
			case '\n': return true;
		}
		return false;
	}

	char to_upper(char ch)
	{
		if(ch >= 'a' && ch <= 'z')
			return ch - 32;

		return ch;
	}

	char to_lower(char ch)
	{
		if(ch >= 'A' && ch <= 'Z')
			return ch + 32;

		return ch;
	}
}
