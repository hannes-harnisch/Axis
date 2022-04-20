#pragma once

#include "Axis/Util/LookupTable.hpp"

#include <cstdint>

namespace ax
{
	// Changing constant values and adding constants requires also changing the lookup table definition below.
	enum class TokenKind : uint8_t
	{
		Name,
		NewLine,
		String,
		Integer,
		Rational,
		Character,
		Comment,

		// One-character tokens
		LBrace,	   // {
		RBrace,	   // }
		LParen,	   // (
		RParen,	   // )
		LBracket,  // [
		RBracket,  // ]
		Colon,	   // :
		Semicolon, // ;
		Comma,	   // ,
		Dot,	   // .
		Backtick,  // `
		Assign,	   // =
		Plus,	   // +
		Minus,	   // -
		Mul,	   // *
		Div,	   // /
		Mod,	   // %
		LogicNot,  // !
		Less,	   // <
		Greater,   // >
		BitAnd,	   // &
		BitOr,	   // |
		Tilde,	   // ~
		At,		   // @
		Pointer,   // ^
		Maybe,	   // ?
		Macro,	   // $

		// Two-character tokens
		Meta,		// ::
		ThinArrow,	// ->
		ThickArrow, // =>
		Range,		// ..
		Increment,	// ++
		Decrement,	// --
		LogicAnd,	// &&
		LogicOr,	// ||
		Equal,		// ==
		Inequal,	// !=
		LessEq,		// <=
		GreaterEq,	// >=
		Pow,		// **
		FloorDiv,	// //
		LShift,		// <<
		RShift,		// >>
		AddAssign,	// +=
		SubAssign,	// -=
		MulAssign,	// *=
		DivAssign,	// /=
		ModAssign,	// %=
		AndAssign,	// &=
		OrAssign,	// |=
		XorAssign,	// ~=

		// Three-character tokens
		Spaceship,		// <=>
		Unpack,			// ...
		RangeInclusive, // ..=
		PowAssign,		// **=
		FloorDivAssign, // //=
		LShiftAssign,	// <<=
		RShiftAssign,	// >>=

		// Keywords
		If,
		Else,
		Switch,
		For,
		Do,
		While,
		Break,
		Continue,
		Return,
		Yield,
		Await,
		Async,
		Try,
		Catch,
		Throw,
		Struct,
		Class,
		Trait,
		Enum,
		Union,
		Public,
		Protected,
		Private,
		Static,
		Dynamic,
		Override,
		Sealed,
		Let,
		Var,
		Const,
		In,
		Use,
		To,
		Interpret,
		Uninit,
		Copy,
		Trust,
		Implicit,

		EndOfFile,
	};

	struct Token
	{
		TokenKind kind	= TokenKind::Name;
		uint16_t length = 0;
		uint32_t offset = 0;

		std::string_view to_string(std::string_view source) const;
	};
}
