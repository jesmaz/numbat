```
Arguments			=>	'(' [ExpressionList] ')'
```
```
Array				=>	'[' [Expression] [ ',' [expression] ] ']'
```
```
Associativity		=>	'ltr'
					|	'rtl'
```
```
BinaryOp			=>	'.' | '*' | '/' | '%' | '+' | '-' | '<<' | '>>' | '&' | '^' | '|' | '<' | '<=' | '>' | '>=' | '==' | '!=' | 'and' | 'or' | ',' | '=' | '+=' | '-=' | '*=' | '/=' | '%=' | '<<=' | '>>=' | '&=' | '^=' | '|=' | '=>'
```
```
Block				=>	'{' (TopLevelExpression;)* '}'
```
```
Call				=>	Arguments
					|	Lvalue [Template] Arguments
					|	PrefixUnaryOp Expression
					|	Expression PostfixUnaryOp
					|	Expression BinaryOp Expression
					|	Expression TernaryOp Expression
```
```
CharArrayLiteral	=>	<string single quotes>
```
```
Decl				=>	ExternDecl
					|	FunctionDecl
					|	OperatorDecl
					|	StructDecl
					|	VariableDecl
```
```
Expression			=>	Call
					|	VariableDecl
					|	Literal
					|	Identifier
					|	'(' Expression ')'

ExpressionList		=>	Expression [',' Expression]*
```
```
ExternDecl			=>	'extern' FunctionDecl
```
```
FloatLiteral		=>	['0' ... '9']* '.' ('0' ... '9')+ [('e' | 'E') ['+' | '-'] ('0' ... '9')+] ['f' | 'F' | 'h' | 'H' | 'q' | 'Q']
```
```
FunctionDecl		=>	'def' [Template] ['~'] FunctionID MetaTag* Paramaters ['->' Paramaters] Block
```
```
FunctionID			=>	Identifier
					|	CharArrayLiteral
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
Literal				=>	CharArrayLiteral
					|	FloatLiteral
					|	IntLiteral
					|	StringLiteral
```
```
Lvalue				=>	Identifier
					|	Call
```
```
MetaTag				=>	'@'Identifier
```
```
##DEPRECATED##
OperatorDecl		=>	'operator' CharArrayLiteral ':' IntLiteral Associativity
```
```
Paramaters			=>	'(' [TypedArgsList] ')'
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
Program				=>	Decl+
```
```
ReturnExpression	=>	'return' [Expression]
```
```
StringLiteral		=>	<string double quotes>
```
```
StructDecl			=>	struct Identifier MetaTag* [Template] TypeParamaters ;
```
```
Template			=>	'[' [ TypedArgsList ] ']'
```
```
Type				=>	[Template] Type [Array]
					|	Identifier
					|	'{' TypedArgsList '}'
					|	'type'
					|	TypeModifier Type
					|	TypeParamaters Paramaters
```
```
TypedArgsList		=>	VariableDecl (',' VariableDecl)*
```
```
TypeModifier		=>	'ref'
					|	'atomic'
					|	'const'
```
```
TypeParamaters		=>	'{' [TypedArgsList] '}'
```
```
VariableDecl		=>	Type Identifier
```