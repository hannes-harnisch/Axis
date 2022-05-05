grammar Axis;

// Parsing rules

compilationUnit		: moduleUse* definition* EOF ;
moduleUse			: USE moduleName ;
moduleName			: NAME ('.' NAME)* ;
definition			: typeDefinition | functionDefinition | aliasDefinition | objectDefinition ;
typeDefinition		: ;

// Lexing rules

NL					: '\n' ;
RATIONAL			: (DEC WS*)? '.' (WS* DEC)? ;
INTEGER				: DEC | HEX | BIN | OCT ;
COMMENT				: '#' .*? '\n' -> channel(HIDDEN) ;
IN					: 'in' ;
USE					: 'use' ;
STRUCT				: 'struct' ;
CLASS				: 'class' ;
ENUM				: 'enum' ;
UNION				: 'union' ;
TRAIT				: 'trait' ;
NAME				: [a-zA-Z_][a-zA-Z_0-9]* ;
SPACE				: WS+ -> skip ;

fragment DEC		: [0-9] (WS* [0-9])* ;
fragment HEX		: '0x' (WS* [0-9A-Fa-f])+ ;
fragment BIN		: '0b' (WS* [01])+ ;
fragment OCT		: '0o' (WS* [0-7])+ ;
fragment WS			: [ \t\r] ;