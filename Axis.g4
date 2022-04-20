grammar Axis;

// Parsing rules

compilationUnit		: moduleUse* definition* EOF ;
moduleUse			: USE moduleName ;
moduleName			: NAME ('.' NAME)* ;
definition			: typeDefinition | functionDefinition | aliasDefinition | objectDefinition ;
typeDefinition		: ;

// Lexing rules

NL					: '\n' ;
RATIONAL			: (DEC NUM_WS)? '.' (NUM_WS DEC)? ;
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

fragment DEC		: [0-9] (NUM_WS [0-9])* ;
fragment HEX		: '0x' (NUM_WS [0-9A-Fa-f])+ ;
fragment BIN		: '0b' (NUM_WS [01])+ ;
fragment OCT		: '0o' (NUM_WS [0-7])+ ;
fragment NUM_WS		: WS* | '\n' ;
fragment WS			: [ \t\r\v\f] ;