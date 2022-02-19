#include "PCH.hpp"

#include "Token.hpp"

namespace ax
{
	std::string_view Token::to_string(std::string_view source) const
	{
		return {&source[offset], length};
	}
}
