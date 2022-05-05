#include "PCH.hpp"

#include "Axis/Util/Char.hpp"
#include "Lexer.hpp"

namespace ax
{
	using enum TokenKind;

	constexpr u16 get_keyword_length(TokenKind kind)
	{
		constexpr int PREFIX_LENGTH = 2;

		auto keyword_length = enum_name(kind).length() - PREFIX_LENGTH;
		return static_cast<u16>(keyword_length);
	}

	constexpr auto TOKEN_LENGTHS = [] {
		LookupTable<TokenKind, u16> _;

		auto increment = [](TokenKind& kind) {
			kind = static_cast<TokenKind>(static_cast<u8>(kind) + 1);
		};

		static_assert(std::to_underlying(LBrace) == 7);
		static_assert(std::to_underlying(Meta) == 34);
		static_assert(std::to_underlying(Spaceship) == 58);
		static_assert(std::to_underlying(EndOfFile) == 103, "You changed the TokenKind enum, but you need to change this "
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

		usize operator()(std::string_view txt) const
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
		TokenStream			   tokens;
		const Source&		   source;
		const Source::Iterator begin;
		const Source::Iterator end;
		Source::Iterator	   cursor;
		Reporter&			   reporter;

	public:
		Lexer(const Source& source, Reporter& reporter) :
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
			u16		  length;

			UnplacedToken() = default;

			UnplacedToken(TokenKind kind) : kind(kind), length(TOKEN_LENGTHS[kind])
			{}

			UnplacedToken(TokenKind kind, u16 length) : kind(kind), length(length)
			{}
		};

		void next_token()
		{
			u32 offset = get_offset();

			UnplacedToken tok;
			switch(*cursor++)
			{
				case ' ':
				case '\t':
				case '\r': return;
				case '\n': tok = NewLine; break;
				case '{': tok = LBrace; break;
				case '}': tok = RBrace; break;
				case '(': tok = LParen; break;
				case ')': tok = RParen; break;
				case '[': tok = LBracket; break;
				case ']': tok = RBracket; break;
				case '#': lex_comment(offset); return;
				case ';': tok = Semicolon; break;
				case ',': tok = Comma; break;
				case '.': match_dot(offset); return;
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
				case '"': lex_string(offset); return;
				case '`': tok = Backtick; break;
				case '$': tok = Macro; break;
				case '\'': lex_character(offset); return;
				case '\\': lex_escaped_keyword(offset); return;
				default: on_alphanumeric(offset); return;
			}
			tokens.append(tok.kind, tok.length, offset);
		}

		bool match(char expected)
		{
			if(cursor == end) [[unlikely]]
				return false;

			if(*cursor != expected)
				return false;

			++cursor;
			return true;
		}

		void on_alphanumeric(u32 offset)
		{
			char ch = cursor[-1];
			if(can_begin_names(ch))
				lex_word(offset);
			else if(is_dec_digit(ch))
				lex_number(offset);
			else
				on_illegal_char();
		}

		void lex_comment(u32 offset)
		{
			auto comment_begin = cursor - 1;
			while(cursor != end)
			{
				if(*cursor++ == '\n')
					break;
			}

			auto length = cursor - comment_begin;
			if(length > UINT16_MAX) [[unlikely]]
				return report<Message::TokenTooLong>(comment_begin);

			tokens.append(Comment, static_cast<u16>(length), offset);
		}

		void lex_quoted_sequence(u32 offset, char quote, TokenKind kind)
		{
			auto seq_begin = cursor - 1;

			while(cursor != end)
			{
				char ch = *cursor++;

				if(ch == quote)
					break;

				if(ch == '\n') [[unlikely]]
				{
					report<Message::MissingClosingQuote>(--cursor);
					break;
				}
			}

			auto length = cursor - seq_begin + 1;
			if(length > UINT16_MAX) [[unlikely]]
				return report<Message::TokenTooLong>(seq_begin);

			tokens.append(kind, static_cast<u16>(length), offset);
		}

		void lex_string(u32 offset)
		{
			lex_quoted_sequence(offset, '"', String);
		}

		void lex_character(u32 offset)
		{
			lex_quoted_sequence(offset, '\'', Character);
		}

		void lex_word(u32 offset)
		{
			auto name_begin = cursor - 1;
			while(cursor != end)
			{
				if(!is_word_char(*cursor))
					break;

				++cursor;
			}

			std::string_view kw(name_begin, cursor);

			auto it	  = KEYWORDS.find(kw);
			auto kind = it == KEYWORDS.end() ? Name : it->second;

			auto length = cursor - name_begin;
			if(length > UINT16_MAX) [[unlikely]]
				return report<Message::TokenTooLong>(name_begin);

			tokens.append(kind, static_cast<u16>(length), offset);
		}

		void lex_escaped_keyword(u32 offset)
		{
			auto name_begin = cursor;
			while(cursor != end)
			{
				if(!is_word_char(*cursor))
					break;

				++cursor;
			}

			std::string_view kw(name_begin, cursor);
			if(KEYWORDS.find(kw) == KEYWORDS.end()) [[unlikely]]
				return report<Message::EscapedNonKeyword>(name_begin, kw);

			tokens.append(Name, static_cast<u16>(kw.length()), offset);
		}

		void lex_number(u32 offset)
		{
			auto num_begin = cursor - 1;
			auto kind	   = determine_number_literal_kind();

			auto length = cursor - num_begin;
			if(length > UINT16_MAX) [[unlikely]]
				return report<Message::TokenTooLong>(num_begin);

			tokens.append(kind, static_cast<u16>(length), offset);
		}

		TokenKind determine_number_literal_kind()
		{
			char first = cursor[-1];
			if(first == '.')
			{
				eat_number_literal<is_dec_digit>();
				return Rational;
			}
			else if(first == '0' && cursor != end)
			{
				switch(*cursor)
				{
					case 'x': eat_number_literal<is_hex_digit>(); return Integer;
					case 'b': eat_number_literal<is_bin_digit>(); return Integer;
					case 'o': eat_number_literal<is_oct_digit>(); return Integer;
				}
			}

			TokenKind kind = Integer;
			while(cursor != end)
			{
				char ch = *cursor++;
				if(ch == '.')
					kind = Rational;

				if(!is_dec_digit(ch) && !is_ignored_whitespace(ch))
					break;
			}
			return kind;
		}

		template<bool (*CHAR_PREDICATE)(char)>
		void eat_number_literal()
		{
			while(cursor != end)
			{
				char ch = *cursor++;
				if(!CHAR_PREDICATE(ch) && !is_ignored_whitespace(ch))
					break;
			}
		}

		void match_dot(u32 offset)
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
			{
				lex_number(offset);
				return;
			}
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

		u32 get_offset() const
		{
			return static_cast<u32>(cursor - begin);
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
