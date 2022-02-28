#pragma once

#include "Axis/Util/LookupTable.hpp"

#include <cstdint>

namespace ax
{
	// Changing constant values and adding constants requires also changing the lookup table definition below.
	enum class TokenKind : uint8_t
	{
		Name,
		String,
		Integer,
		Rational,
		Comment,

		// One-character tokens
		LBrace,
		RBrace,
		LParen,
		RParen,
		LBracket,
		RBracket,
		Colon,
		Semicolon,
		Comma,
		Dot,
		Macro,
		Escape,
		Assign,
		Plus,
		Minus,
		Mul,
		Div,
		Mod,
		LogicNot,
		Less,
		Greater,
		BitAnd,
		BitOr,
		Tilde,
		Address,
		Pointer,
		Maybe,

		// Two-character tokens
		Meta,
		Arrow,
		Conversion,
		Range,
		Increment,
		Decrement,
		LogicAnd,
		LogicOr,
		Equal,
		Inequal,
		LessEq,
		GreaterEq,
		Pow,
		FloorDiv,
		LShift,
		RShift,
		AddAssign,
		SubAssign,
		MulAssign,
		DivAssign,
		ModAssign,
		AndAssign,
		OrAssign,
		XorAssign,

		// Three-character tokens
		Spaceship,
		Unpack,
		RangeInclusive,
		PowAssign,
		FloorDivAssign,
		LShiftAssign,
		RShiftAssign,

		// Keywords
		KwI8,
		KwI16,
		KwI32,
		KwI64,
		KwI128,
		KwISize,
		KwU8,
		KwU16,
		KwU32,
		KwU64,
		KwU128,
		KwUSize,
		KwF16,
		KwF32,
		KwF64,
		KwF128,
		KwFSize,
		KwByte,
		KwChar,
		KwBool,
		KwIf,
		KwElse,
		KwWhen,
		KwFor,
		KwDo,
		KwWhile,
		KwBreak,
		KwContinue,
		KwReturn,
		KwYield,
		KwAsync,
		KwAwait,
		KwTry,
		KwCatch,
		KwThrow,
		KwTuple,
		KwClass,
		KwConcept,
		KwEnum,
		KwUnion,
		KwOperator,
		KwOut,
		KwIn,
		KwPack,
		KwFamily,
		KwFriend,
		KwStatic,
		KwDynamic,
		KwOverride,
		KwFinal,
		KwLet,
		KwVar,
		KwConst,
		KwUninit,
		KwOwn,
		KwCopy,
		KwTrue,
		KwFalse,
		KwNull,
		KwThis,
		KwTrust,

		EndOfFile,
	};

	struct Token
	{
		TokenKind kind	 = TokenKind::Name;
		uint16_t  length = 0;
		uint32_t  offset = 0;

		std::string_view to_string(std::string_view source) const;
	};
}
