```
BinaryOp			=>	'.' | '*' | '/' | '%' | '+' | '-' | '<<' | '>>' | '&' | '^' | '|' | '<' | '<=' | '>' | '>=' | '==' | '!=' | 'and' | 'as' | 'is' | 'or' | ',' | '=' | '+=' | '-=' | '*=' | '/=' | '%=' | '<<=' | '>>=' | '&=' | '^=' | '|=' | '=>'
```
```
Body				=>	Expression ; BodyCont
					|	;
```
```
BodyCont			=>	Expression ; BodyCont
					|	ε
```
```
BracketCurvy		=>	'{' '}'
					|	'{' Body '}'
					|	'{' Expression '}'
```
```
BracketRound		=>	'(' ')'
					|	'(' Body ')'
					|	'(' Expression ')'
```
```
BracketSquare		=>	'[' ']'
					|	'[' Expression ']'
```
```
Call				=>	Template Expression BracketRound
					|	PrefixUnaryOp Expression
					|	Expression PostfixUnaryOp
					|	Expression BinaryOp Expression
					|	Expression TernaryOp Expression
```
```
CharArrayLiteral	=>	<string single quotes>
```
```
Decl				=>	Expression ;
					|	ExternDecl
					|	FunctionDecl
					|	StructDecl
					|	TypeDef
```
```
Expression			=>	BracketCurvy
					|	BracketRound
					|	Call
					|	Identifier
					|	LambdaExpression
					|	Literal
					|	Type
					|	VariableDecl
```
```
ExternDecl			=>	'extern' FunctionDecl
```
```
FloatLiteral		=>	['0' ... '9']* '.' ('0' ... '9')+ [('e' | 'E') ['+' | '-'] ('0' ... '9')+] ['f' | 'F' | 'h' | 'H' | 'q' | 'Q']
```
```
FunctionDecl		=>	'def' Template FunctionID LambdaExpression ;
```
```
FunctionID			=>	'~' CharArrayLiteral
					|	'~' Identifier
					|	CharArrayLiteral
					|	Identifier
```
```
Identifier			=>	ID_start [ID_continue]*
```
```
ID_start			=>	<letters>
					|	'_'
```
```
ID_continue			=>	ID_start
					|	<numbers>
```
```
IntLiteral			=>	['1' ... '9'] ('0' ... '9')+
					|	'0' ('b' | 'B') ('0' | '1')+
					|	'0' ('o' | 'O') ('0' ... '7')+
					|	'0' ('x' | 'X') ('0' ... '9' | 'a' ... 'f' | 'A' ... 'F')+
```
```
LambdaExpression	=>	BracketRound BracketCurvy
					|	BracketRound '->' BracketRound
					|	BracketRound '->' BracketRound BracketCurvy
```
```
Literal				=>	CharArrayLiteral
					|	FloatLiteral
					|	IntLiteral
					|	StringLiteral
```
```
MetaTag				=>	'@'Identifier
```
```
MetaTagList			=>	MetaTag MetaTagList
					|	ε
```
```
PostfixUnaryOp		=>	'++' | '--'
```
```
PrefixUnaryOp		=>	'++' | '--' | '+' | '-' | '!' | 'not' | '~'
```
```
TopLevelExpression	=>	Expression
					|	ReturnExpression
```
```
Program				=>	Decl Program
					|	ε
```
```
ReturnExpression	=>	'return'
					|	'return' Expression
```
```
StringLiteral		=>	<string double quotes>
```
```
StructDecl			=>	struct Template Identifier MetaTagList BracketCurvy ;
```
```
Template			=>	BracketSquare
					|	ε
```
```
Type				=>	BracketCurvy
					|	BracketRound '->' BracketRound
					|	Identifier
					|	'type'
					|	TypeModifier Type
```
```
TypeDef				=>	'typedef' VariableDecl
```
```
TypeModifier		=>	'ref'
					|	'atomic'
					|	'const'
```
```
VariableDecl		=>	Type Identifier
```
