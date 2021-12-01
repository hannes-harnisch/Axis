#include "PCH.hpp"

#include "Lexer.hpp"

namespace ax
{
	class Lexer
	{
		std::string::const_iterator begin;
		std::string::const_iterator current;
		std::string::const_iterator end;

	public:
		Lexer(std::string const& file_content) : begin(file_content.begin()), current(begin), end(file_content.end())
		{}

		std::vector<Token> lex()
		{
			std::vector<Token> tokens;
			while(current != end)
				tokens.emplace_back(next_token());

			tokens.emplace_back(Token {
				.offset = static_cast<uint32_t>(end - begin),
				.length = 0,
				.kind	= TokenKind::EndOfFile,
			});
			return tokens;
		}

	private:
		Token next_token()
		{
			uint32_t  offset = static_cast<uint32_t>(current - begin);
			TokenKind kind;
			switch(*current++)
			{
				case '{': return {offset, 1, TokenKind::LBrace};
				case '}': return {offset, 1, TokenKind::RBrace};
				case '(': return {offset, 1, TokenKind::LParen};
				case ')': return {offset, 1, TokenKind::RParen};
				case '[': return {offset, 1, TokenKind::LBracket};
				case ']': return {offset, 1, TokenKind::RBracket};
				case ';': return {offset, 1, TokenKind::Semicolon};
				case ',': return {offset, 1, TokenKind::Comma};
			}
		}
	};

	std::vector<Token> lex(std::string const& file_content)
	{
		return Lexer(file_content).lex();
	}
}
