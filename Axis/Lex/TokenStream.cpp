#include "PCH.hpp"

#include "TokenStream.hpp"

namespace ax
{
	void TokenStream::append(TokenKind kind, uint16_t length, uint32_t offset)
	{
		tokens.emplace_back(kind, length, offset);
	}

	std::string TokenStream::to_string(std::string_view source) const
	{
		constexpr unsigned AVG_TOKEN_LENGTH = 5; // arbitrary heuristic

		std::string str;
		str.reserve(tokens.size() * AVG_TOKEN_LENGTH);

		for(auto token : tokens)
			str += std::format("`{}` ", token.to_string(source));

		return str;
	}
}
