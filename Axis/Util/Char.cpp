#include "PCH.hpp"

#include "Char.hpp"

namespace ax
{
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

	bool is_ignored_whitespace(char ch)
	{
		switch(ch)
		{
			case ' ':
			case '\t':
			case '\r':
			case '\n':
			case '\v':
			case '\f': return true;
		}
		return false;
	}

	bool is_word_char(char ch)
	{
		return ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || is_dec_digit(ch) || ch == '_';
	}

	bool can_begin_names(char ch)
	{
		return ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch == '_';
	}

	bool is_dec_digit(char ch)
	{
		return ch >= '0' && ch <= '9';
	}

	bool is_hex_digit(char ch)
	{
		return is_dec_digit(ch) || ch >= 'A' && ch <= 'F' || ch >= 'a' && ch <= 'f';
	}

	bool is_oct_digit(char ch)
	{
		return ch >= '0' && ch <= '7';
	}

	bool is_bin_digit(char ch)
	{
		return ch == '0' || ch == '1';
	}
}
