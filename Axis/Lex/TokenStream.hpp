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
		void		append(TokenKind kind, uint16_t length, uint32_t offset);
		std::string to_string(std::string_view source) const;

	private:
		std::vector<Token> tokens;
	};
}
