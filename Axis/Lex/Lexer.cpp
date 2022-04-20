#include "PCH.hpp"

#include "Axis/Util/Char.hpp"
#include "Lexer.hpp"

namespace ax
{
	using enum TokenKind;

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

		static_assert(std::to_underlying(LBrace) == 7);
		static_assert(std::to_underlying(Meta) == 34);
		static_assert(std::to_underlying(Spaceship) == 58);
		static_assert(std::to_underlying(EndOfFile) == 102, "You changed the TokenKind enum, but you need to change this "
															"lookup table computation to adapt to your changes.");
		for(auto i = LBrace; i != Meta; increment(i))
			_[i] = 1;

		for(auto i = Meta; i != Spaceship; increment(i))
			_[i] = 2;

		for(auto i = Spaceship; i != If; increment(i))
			_[i] = 3;

		for(auto i = If; i != EndOfFile; increment(i))
			_[i] = get_keyword_length(i);

		return _;
	}();

	struct StringHash
	{
		using is_transparent = void;

		size_t operator()(std::string_view txt) const
		{
			return std::hash<std::string_view>()(txt);
		}
	};

	static const std::unordered_map<std::string, TokenKind, StringHash, std::equal_to<>> KEYWORDS {
		{"if", If},			  {"else", Else},			{"switch", Switch},	  {"for", For},
		{"do", Do},			  {"while", While},			{"break", Break},	  {"continue", Continue},
		{"return", Return},	  {"yield", Yield},			{"await", Await},	  {"async", Async},
		{"try", Try},		  {"catch", Catch},			{"throw", Throw},	  {"struct", Struct},
		{"class", Class},	  {"trait", Trait},			{"enum", Enum},		  {"union", Union},
		{"public", Public},	  {"protected", Protected}, {"private", Private}, {"static", Static},
		{"dynamic", Dynamic}, {"override", Override},	{"sealed", Sealed},	  {"let", Let},
		{"var", Var},		  {"const", Const},			{"in", In},			  {"use", Use},
		{"to", To},			  {"interpret", Interpret}, {"uninit", Uninit},	  {"copy", Copy},
		{"trust", Trust},	  {"implicit", Implicit},
	};

	class Lexer
	{
		TokenStream tokens;
		Source const& source;
		Source::Iterator const begin;
		Source::Iterator const end;
		Source::Iterator cursor;
		Reporter& reporter;

	public:
		Lexer(Source const& source, Reporter& reporter) :
			source(source), begin(source.begin()), end(source.end()), cursor(begin), reporter(reporter)
		{}

		TokenStream lex()
		{
			while(cursor != end)
				next_token();

			tokens.append(TokenKind::EndOfFile, 0, 0);
			return std::move(tokens);
		}

	private:
		struct UnplacedToken
		{
			TokenKind kind;
			uint16_t length;

			UnplacedToken() = default;

			UnplacedToken(TokenKind kind) : kind(kind), length(TOKEN_LENGTHS[kind])
			{}

			UnplacedToken(TokenKind kind, uint16_t length) : kind(kind), length(length)
			{}
		};

		void next_token()
		{
			uint32_t offset = get_offset();
			char character	= *cursor++;

			UnplacedToken tok;
			switch(character)
			{
				case ' ':
				case '\t':
				case '\r':
				case '\f':
				case '\v': return;
				case '\n': tok = NewLine; break;
				case '{': tok = LBrace; break;
				case '}': tok = RBrace; break;
				case '(': tok = LParen; break;
				case ')': tok = RParen; break;
				case '[': tok = LBracket; break;
				case ']': tok = RBracket; break;
				case '#': return lex_comment(offset);
				case ';': tok = Semicolon; break;
				case ',': tok = Comma; break;
				case '.': return match_dot(offset);
				case ':': tok = match_colon(); break;
				case '=': tok = match_equal(); break;
				case '+': tok = match_plus(); break;
				case '-': tok = match_minus(); break;
				case '*': tok = match_star(); break;
				case '/': tok = match_slash(); break;
				case '!': tok = match_exclamation(); break;
				case '&': tok = match_and(); break;
				case '|': tok = match_or(); break;
				case '^': tok = Pointer; break;
				case '@': tok = At; break;
				case '<': tok = match_left_angle(); break;
				case '>': tok = match_right_angle(); break;
				case '?': tok = Maybe; break;
				case '%': tok = match_percent(); break;
				case '~': tok = match_tilde(); break;
				case '"': return lex_string(offset);
				case '`': tok = Backtick; break;
				case '$': tok = Macro; break;
				case '\'': return lex_character(offset);
				case '\\': return lex_escaped_keyword(offset);
				default: return on_alphanumeric(offset, character);
			}
			tokens.append(tok.kind, tok.length, offset);
		}

		bool match(char expected)
		{
			if(cursor == end)
				return false;

			if(*cursor != expected)
				return false;

			++cursor;
			return true;
		}

		void on_alphanumeric(uint32_t offset, char character)
		{
			if(can_begin_names(character))
				lex_word(offset);
			else if(is_dec_digit(character))
				lex_number(offset, character);
			else
				on_illegal_char();
		}

		void lex_comment(uint32_t offset)
		{
			auto comment_begin = cursor - 1;
			while(*cursor++ != '\n')
			{}

			auto length = cursor - comment_begin;
			if(length > UINT16_MAX) [[unlikely]]
				return report<Message::TokenTooLong>(comment_begin);

			tokens.append(Comment, static_cast<uint16_t>(length), offset);
		}

		void lex_string(uint32_t offset)
		{
			auto string_begin = cursor;

			char ch = *cursor++;
			while(true)
			{
				if(ch == '"')
					break;

				if(ch == '\n')
				{
					report<Message::MissingClosingQuote>(cursor - 1);
					break;
				}
				ch = *cursor++;
			}

			auto length = cursor - string_begin + 1;
			if(length > UINT16_MAX) [[unlikely]]
				return report<Message::StringTooLong>(string_begin - 1);

			tokens.append(String, static_cast<uint16_t>(length), offset);
		}

		void lex_character(uint32_t offset)
		{
			auto char_begin = cursor;
			char ch			= *cursor++;
			while(true)
			{
				if(ch == '\'')
					break;

				if(ch == '\n')
				{
					report<Message::MissingClosingQuote>(cursor - 1);
					break;
				}
				ch = *cursor++;
			}

			auto length = cursor - char_begin + 1;
			if(length > UINT16_MAX) [[unlikely]]
				return report<Message::TokenTooLong>(char_begin - 1);

			tokens.append(Character, static_cast<uint16_t>(length), offset);
		}

		void lex_word(uint32_t offset)
		{
			auto name_begin = --cursor;
			while(is_word_char(*++cursor))
			{}

			std::string_view kw(name_begin, cursor);

			auto it	  = KEYWORDS.find(kw);
			auto kind = it == KEYWORDS.end() ? Name : it->second;

			auto length = cursor - name_begin;
			if(length > UINT16_MAX) [[unlikely]]
				return report<Message::TokenTooLong>(name_begin);

			tokens.append(kind, static_cast<uint16_t>(length), offset);
		}

		void lex_escaped_keyword(uint32_t offset)
		{
			auto name_begin = cursor;
			while(is_word_char(*cursor++))
			{}

			std::string_view kw(name_begin, --cursor);
			if(KEYWORDS.find(kw) == KEYWORDS.end()) [[unlikely]]
				return report<Message::EscapedNonKeyword>(name_begin - 1, kw);

			tokens.append(Name, static_cast<uint16_t>(kw.length()), offset);
		}

		void lex_number(uint32_t offset, char first)
		{
			auto number_begin = cursor - 1;

			auto kind = Integer;
			determine_number_literal_kind(kind, first);

			auto length = --cursor - number_begin;
			if(length > UINT16_MAX) [[unlikely]]
				return report<Message::TokenTooLong>(number_begin);

			tokens.append(kind, static_cast<uint16_t>(length), offset);
		}

		void determine_number_literal_kind(TokenKind& kind, char first)
		{
			if(first == '.')
			{
				kind = Rational;
				traverse_number_literal<is_dec_digit>();
				return;
			}
			else if(first == '0')
			{
				switch(*cursor)
				{
					case 'x': return traverse_number_literal<is_hex_digit>();
					case 'b': return traverse_number_literal<is_bin_digit>();
					case 'o': return traverse_number_literal<is_oct_digit>();
				}
			}

			char ch;
			do
			{
				ch = *cursor++;
				if(ch == '.')
					kind = Rational;
			} while(is_dec_digit(ch) || is_whitespace(ch));
		}

		template<bool (*CHAR_PREDICATE)(char)>
		void traverse_number_literal()
		{
			char ch;
			do
				ch = *cursor++;
			while(CHAR_PREDICATE(ch) || is_whitespace(ch));
		}

		void match_dot(uint32_t offset)
		{
			UnplacedToken tok;
			if(match('.'))
			{
				if(match('.'))
					tok = Unpack;
				else if(match('='))
					tok = RangeInclusive;
				else
					tok = Range;
			}
			else if(is_dec_digit(*cursor))
				return lex_number(offset, '.');
			else
				tok = Dot;

			tokens.append(tok.kind, tok.length, offset);
		}

		UnplacedToken match_colon()
		{
			if(match(':'))
				return Meta;
			else
				return Colon;
		}

		UnplacedToken match_equal()
		{
			if(match('='))
				return Equal;
			else if(match('>'))
				return ThickArrow;
			else
				return Assign;
		}

		UnplacedToken match_plus()
		{
			if(match('+'))
				return Increment;
			else if(match('='))
				return AddAssign;
			else
				return Plus;
		}

		UnplacedToken match_minus()
		{
			if(match('>'))
				return ThinArrow;
			else if(match('-'))
				return Decrement;
			else if(match('='))
				return SubAssign;
			else
				return Minus;
		}

		UnplacedToken match_star()
		{
			if(match('*'))
			{
				if(match('='))
					return PowAssign;
				else
					return Pow;
			}
			else if(match('='))
				return MulAssign;
			else
				return Mul;
		}

		UnplacedToken match_slash()
		{
			if(match('/'))
			{
				if(match('='))
					return FloorDivAssign;
				else
					return FloorDiv;
			}
			else if(match('='))
				return DivAssign;
			else
				return Div;
		}

		UnplacedToken match_exclamation()
		{
			if(match('='))
				return Inequal;
			else
				return LogicNot;
		}

		UnplacedToken match_and()
		{
			if(match('&'))
				return LogicAnd;
			else if(match('='))
				return AndAssign;
			else
				return BitAnd;
		}

		UnplacedToken match_or()
		{
			if(match('|'))
				return LogicOr;
			else if(match('='))
				return OrAssign;
			else
				return BitOr;
		}

		UnplacedToken match_left_angle()
		{
			if(match('<'))
			{
				if(match('='))
					return LShiftAssign;
				else
					return LShift;
			}
			else if(match('='))
			{
				if(match('>'))
					return Spaceship;
				else
					return LessEq;
			}
			else
				return Less;
		}

		UnplacedToken match_right_angle()
		{
			if(match('>'))
			{
				if(match('='))
					return RShiftAssign;
				else
					return RShift;
			}
			else if(match('='))
				return GreaterEq;
			else
				return Greater;
		}

		UnplacedToken match_percent()
		{
			if(match('='))
				return ModAssign;
			else
				return Mod;
		}

		UnplacedToken match_tilde()
		{
			if(match('='))
				return XorAssign;
			else
				return Tilde;
		}

		void on_illegal_char() const
		{
			report<Message::IllegalChar>(cursor - 1, static_cast<int>(cursor[-1]));
		}

		uint32_t get_offset() const
		{
			return static_cast<uint32_t>(cursor - begin);
		}

		template<Message MSG, typename... Ts>
		void report(Source::Iterator loc, Ts&&... ts) const
		{
			reporter.report<MSG>(source.locate(loc), std::forward<Ts>(ts)...);
		}
	};

	TokenStream lex(Source const& source, Reporter& reporter)
	{
		return Lexer(source, reporter).lex();
	}
}
