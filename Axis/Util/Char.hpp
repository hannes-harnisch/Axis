#pragma once

namespace ax
{
	char to_upper(char ch);
	char to_lower(char ch);
	bool is_ignored_whitespace(char ch);
	bool is_word_char(char ch);
	bool can_begin_names(char ch);
	bool is_dec_digit(char ch);
	bool is_hex_digit(char ch);
	bool is_oct_digit(char ch);
	bool is_bin_digit(char ch);
}
