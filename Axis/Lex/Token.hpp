#pragma once

#include <cstdint>

namespace ax
{
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
		LAngle,
		RAngle,
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
		BitAnd,
		BitOr,
		Tilde,
		LogicNot,
		Address,
		Pointer,
		Maybe,

		// Two-character tokens
		Meta,
		Arrow,
		Conversion,
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
		Ellipsis,
		PowAssign,
		FloorDivAssign,
		LShiftAssign,
		RShiftAssign,

		// Keywords
		KwInt,
		KwInt8,
		KwInt16,
		KwInt32,
		KwInt64,
		KwInt128,
		KwUInt,
		KwUInt8,
		KwUInt16,
		KwUInt32,
		KwUInt64,
		KwUInt128,
		KwFloat,
		KwFloat16,
		KwFloat32,
		KwFloat64,
		KwFloat128,
		KwFloatX,
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
		KwVirtual,
		KwOverride,
		KwFinal,
		KwStatic,
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

		EndOfFile,
	};

	struct Token
	{
		uint32_t  offset = 0;
		uint16_t  length = 0;
		TokenKind kind	 = TokenKind::Name;

		std::string_view get_view(std::string_view source) const
		{
			return {&source[offset], length};
		}
	};
}
