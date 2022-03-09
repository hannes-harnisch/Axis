grammar Axis;

// Parsing rules

// Lexing rules

RATIONAL		: (DEC WS*)? '.' (WS* DEC)? ;
INTEGER			: DEC | HEX | BIN | OCT ;
COMMENT			: '#' .*? '\n' ;

fragment DEC	: [0-9] (WS* [0-9])* ;
fragment HEX	: '0x' (WS* [0-9A-Fa-f])+ ;
fragment BIN	: '0b' (WS* [01])+ ;
fragment OCT	: '0o' (WS* [0-7])+ ;
fragment WS		: [ \t\n\r] ;
WHITESPACE		: WS+ -> skip ;
