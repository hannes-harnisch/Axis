#pragma once

#include "Token.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace ax
{
	class TokenStream
	{
	public:
		void		append(TokenKind kind, u16 length, u32 offset);
		std::string to_string(std::string_view source) const;

	private:
		std::vector<Token> tokens;
	};
}
