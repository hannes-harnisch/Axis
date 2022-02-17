#include "PCH.hpp"

#include "Lexer.hpp"

namespace ax
{
	constexpr uint16_t get_keyword_length(TokenKind kind)
	{
		constexpr int PREFIX_LENGTH = 2;

		auto keyword_length = enum_name(kind).length() - PREFIX_LENGTH;
		return static_cast<uint16_t>(keyword_length);
	}

	constexpr auto TOKEN_LENGTHS = [] {
		LookupTable<TokenKind, uint16_t> _;

		auto increment = [](TokenKind& kind) {
			kind = static_cast<TokenKind>(static_cast<uint8_t>(kind) + 1);
		};

		using enum TokenKind;
		static_assert(std::to_underlying(LBrace) == 5);
		static_assert(std::to_underlying(Meta) == 32);
		static_assert(std::to_underlying(Spaceship) == 55);
		static_assert(std::to_underlying(EndOfFile) == 122, "You changed the TokenKind enum, but you need to change this "
															"lookup table computation to adapt to your changes.");
		for(auto i = LBrace; i != Meta; increment(i))
			_[i] = 1;

		for(auto i = Meta; i != Spaceship; increment(i))
			_[i] = 2;

		for(auto i = Spaceship; i != KwInt; increment(i))
			_[i] = 3;

		for(auto i = KwInt; i != EndOfFile; increment(i))
			_[i] = get_keyword_length(i);

		return _;
	}();

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
			{
				uint32_t offset		= get_offset();
				auto [kind, length] = next_token();
				tokens.emplace_back(kind, length, offset);
			}

			tokens.emplace_back(Token {
				.kind	= TokenKind::EndOfFile,
				.length = 0,
				.offset = get_offset(),
			});
			return tokens;
		}

	private:
		struct UnplacedToken
		{
			TokenKind kind	 = TokenKind::Name;
			uint16_t  length = TOKEN_LENGTHS[kind];
		};

		using enum TokenKind;

		UnplacedToken next_token()
		{
			switch(*current++)
			{
				case '{': return {LBrace};
				case '}': return {RBrace};
				case '(': return {LParen};
				case ')': return {RParen};
				case '[': return {LBracket};
				case ']': return {RBracket};
				case ';': return {Semicolon};
				case ',': return {Comma};
				case '=': return match_equal();
				case '+': return match_plus();
				case '-': return match_minus();
				case '*': return match_star();
				case '/': return match_slash();
				case '<': return match_left_angle();
				case '>': return match_right_angle();
			}
		}

		bool match(char expected)
		{
			if(current == end)
				return false;

			if(*current != expected)
				return false;

			++current;
			return true;
		}

		UnplacedToken match_equal()
		{
			if(match('='))
				return {Equal};
			else if(match('>'))
				return {Conversion};
			else
				return {Assign};
		}

		UnplacedToken match_plus()
		{
			if(match('+'))
				return {Increment};
			else if(match('='))
				return {AddAssign};
			else
				return {Plus};
		}

		UnplacedToken match_minus()
		{
			if(match('-'))
				return {Decrement};
			else if(match('='))
				return {SubAssign};
			else
				return {Minus};
		}

		UnplacedToken match_star()
		{
			if(match('*'))
			{
				if(match('='))
					return {PowAssign};
				else
					return {Pow};
			}
			else if(match('='))
				return {MulAssign};
			else
				return {Mul};
		}

		UnplacedToken match_slash()
		{
			if(match('/'))
			{
				if(match('='))
					return {FloorDivAssign};
				else
					return {FloorDiv};
			}
			else if(match('='))
				return {DivAssign};
			else
				return {Div};
		}

		UnplacedToken match_left_angle()
		{
			if(match('<'))
			{
				if(match('='))
					return {LShiftAssign};
				else
					return {LShift};
			}
			else if(match('='))
			{
				if(match('>'))
					return {Spaceship};
				else
					return {LessEq};
			}
			else
				return {Less};
		}

		UnplacedToken match_right_angle()
		{
			if(match('>'))
			{
				if(match('='))
					return {RShiftAssign};
				else
					return {RShift};
			}
			else if(match('='))
				return {GreaterEq};
			else
				return {Greater};
		}

		uint32_t get_offset() const
		{
			return static_cast<uint32_t>(current - begin);
		}
	};

	std::vector<Token> lex(std::string const& file_content)
	{
		return Lexer(file_content).lex();
	}
}
