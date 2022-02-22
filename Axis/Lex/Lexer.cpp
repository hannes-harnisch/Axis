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

		static_assert(std::to_underlying(LBrace) == 5);
		static_assert(std::to_underlying(Meta) == 32);
		static_assert(std::to_underlying(Spaceship) == 56);
		static_assert(std::to_underlying(EndOfFile) == 124, "You changed the TokenKind enum, but you need to change this "
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

	const std::unordered_map<std::string, TokenKind> KEYWORD_MAP {
		{"int", KwInt},			  {"int8", KwInt8},
		{"int16", KwInt16},		  {"int32", KwInt32},
		{"int64", KwInt64},		  {"int128", KwInt128},
		{"uint", KwUInt},		  {"uint8", KwUInt8},
		{"uint16", KwUInt16},	  {"uint32", KwUInt32},
		{"uint64", KwUInt64},	  {"uint128", KwUInt128},
		{"float", KwFloat},		  {"float16", KwFloat16},
		{"float32", KwFloat32},	  {"float64", KwFloat64},
		{"float128", KwFloat128}, {"floatX", KwFloatX},
		{"byte", KwByte},		  {"char", KwChar},
		{"bool", KwBool},		  {"if", KwIf},
		{"else", KwElse},		  {"when", KwWhen},
		{"for", KwFor},			  {"do", KwDo},
		{"while", KwWhile},		  {"break", KwBreak},
		{"continue", KwContinue}, {"return", KwReturn},
		{"yield", KwYield},		  {"async", KwAsync},
		{"await", KwAwait},		  {"try", KwTry},
		{"catch", KwCatch},		  {"throw", KwThrow},
		{"tuple", KwTuple},		  {"class", KwClass},
		{"concept", KwConcept},	  {"enum", KwEnum},
		{"union", KwUnion},		  {"operator", KwOperator},
		{"out", KwOut},			  {"in", KwIn},
		{"pack", KwPack},		  {"family", KwFamily},
		{"friend", KwFriend},	  {"virtual", KwVirtual},
		{"override", KwOverride}, {"final", KwFinal},
		{"static", KwStatic},	  {"let", KwLet},
		{"var", KwVar},			  {"const", KwConst},
		{"uninit", KwUninit},	  {"own", KwOwn},
		{"copy", KwCopy},		  {"true", KwTrue},
		{"false", KwFalse},		  {"null", KwNull},
		{"this", KwThis},
	};

	class Lexer
	{
		Source const&					  source;
		std::string::const_iterator const begin;
		std::string::const_iterator const end;
		std::string::const_iterator		  cursor;
		Reporter&						  reporter;

	public:
		Lexer(Source const& source, Reporter& reporter) :
			source(source), begin(source.begin()), end(source.end()), cursor(begin), reporter(reporter)
		{}

		TokenStream lex()
		{
			TokenStream tokens;
			while(cursor != end)
			{
				uint32_t offset = get_offset();
				auto	 token	= next_token();
				if(token)
					tokens.append({token->kind, token->length, offset});
			}

			tokens.append({
				.kind	= TokenKind::EndOfFile,
				.length = 0,
				.offset = 0,
			});
			return tokens;
		}

	private:
		struct UnplacedToken
		{
			TokenKind kind;
			uint16_t  length;

			UnplacedToken(TokenKind kind) : kind(kind), length(TOKEN_LENGTHS[kind])
			{}

			UnplacedToken(TokenKind kind, uint16_t length) : kind(kind), length(length)
			{}
		};

		std::optional<UnplacedToken> next_token()
		{
			char character = *cursor++;
			switch(character)
			{
				case ' ':
				case '\t':
				case '\r':
				case '\n': break;
				case '{': return LBrace;
				case '}': return RBrace;
				case '(': return LParen;
				case ')': return RParen;
				case '[': return LBracket;
				case ']': return RBracket;
				case '#': return lex_comment();
				case ';': return Semicolon;
				case ',': return Comma;
				case '.': return match_dot();
				case ':': return match_colon();
				case '=': return match_equal();
				case '+': return match_plus();
				case '-': return match_minus();
				case '*': return match_star();
				case '/': return match_slash();
				case '!': return match_exclamation();
				case '&': return match_and();
				case '|': return match_or();
				case '^': return Pointer;
				case '@': return Address;
				case '<': return match_left_angle();
				case '>': return match_right_angle();
				case '?': return Maybe;
				case '%': return match_percent();
				case '~': return match_tilde();
				case '"': return lex_string();
				case '$': return Macro;
				case '\\': return lex_escaped_keyword();
				default:
					if(can_begin_names(character))
						return lex_word();
					else if(is_dec_digit(character))
						return lex_number(character);
					else
						on_illegal_char();
			}
			return {};
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

		UnplacedToken lex_comment()
		{
			auto comment_begin = cursor;
			while(*cursor++ != '\n')
			{}

			auto length = static_cast<uint16_t>(cursor - comment_begin);
			return {Comment, length};
		}

		UnplacedToken match_dot()
		{
			if(match('.'))
			{
				if(match('.'))
					return Unpack;
				else if(match('='))
					return RangeInclusive;
				else
					return Range;
			}
			else if(is_dec_digit(*cursor))
				return lex_number('.');
			else
				return Dot;
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
				return Conversion;
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
				return Arrow;
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

		UnplacedToken lex_string()
		{
			auto string_begin = cursor;
			char ch			  = *cursor++;

			while(true)
			{
				if(ch == '"')
					break;

				if(ch == '\n')
				{
					report<Message::UnterminatedString>();
					break;
				}
				ch = *cursor++;
			}

			auto length = ++cursor - string_begin;
			if(length > UINT16_MAX)
				report<Message::LiteralTooLong>();

			return {String, static_cast<uint16_t>(length)};
		}

		UnplacedToken lex_word()
		{
			auto name_begin = --cursor;
			while(is_word_char(*++cursor))
			{}

			auto it	  = KEYWORD_MAP.find(std::string(name_begin, cursor));
			auto kind = it == KEYWORD_MAP.end() ? Name : it->second;

			auto length = cursor - name_begin;
			if(length > UINT16_MAX)
				report<Message::NameTooLong>();

			return {kind, static_cast<uint16_t>(length)};
		}

		UnplacedToken lex_escaped_keyword()
		{
			auto name_begin = --cursor;
			while(is_word_char(*++cursor))
			{}

			std::string_view kw(name_begin, cursor);

			auto it = KEYWORD_MAP.find(std::string(kw));
			if(it == KEYWORD_MAP.end())
				report<Message::EscapedNonKeyword>(kw);

			return {Name, static_cast<uint16_t>(cursor - name_begin)};
		}

		UnplacedToken lex_number(char first)
		{
			auto number_begin = cursor - 1;

			auto kind = Integer;
			determine_number_literal_kind(kind, first);

			auto length = cursor - number_begin;
			if(length > UINT16_MAX)
				report<Message::LiteralTooLong>();

			return {kind, static_cast<uint16_t>(length)};
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

		template<bool (*CHAR_PREDICATE)(char)> void traverse_number_literal()
		{
			char ch;
			do
				ch = *cursor++;
			while(CHAR_PREDICATE(ch) || is_whitespace(ch));
		}

		void on_illegal_char() const
		{
			report<Message::IllegalChar>(cursor[-1]);
		}

		uint32_t get_offset() const
		{
			return static_cast<uint32_t>(cursor - begin);
		}

		template<Message MSG, typename... Ts> void report(Ts&&... ts) const
		{
			reporter.report<MSG>(source.locate(cursor), std::forward<Ts>(ts)...);
		}
	};

	TokenStream lex(Source const& source, Reporter& reporter)
	{
		return Lexer(source, reporter).lex();
	}
}
