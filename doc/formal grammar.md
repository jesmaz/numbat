```
Atom				=>	'{' Body '}'
					|	'(' List ')'
					|	Atom '(' Arguments ')'
					|	Atom '.' Identifier
					|	Atom Slice
					|	Do
					|	For
					|	Identifier
					|	IfElse
					|	Literal
					|	While
```
```
Arguments			=>	Identifier ':' Expression
					|	Identifier ':' Expression ',' Arguments
					|	Expression
					|	Expression ',' Arguments
					|	ε
```
```
Assignment			=>	List AssignOpp List
					|	List AssignOpp Assignment
```
```
AssignOpp			=>	'=' | '+=' | '-=' | '*=' | '/=' | '%=' | '<<=' | '>>=' | '&=' | '^=' | '|=' | '=>'
```
```
BinaryOp			=>	'.' | '*' | '/' | '%' | '+' | '-' | '<<' | '>>' | '&' | '^' | '|' | '<' | '<=' | '>' | '>=' | '==' | '!=' | 'and' | 'as' | 'is' | 'or'
```
```
Body				=>	Body Statement
					|	Body MetaTags Statement
					|	ε
```
```
CharArrayLiteral	=>	<string single quotes>
```
```
Decl				=>	Statement
					|	MetaTags Statement
```
```
Expression			=>	Atom
					|	Atom AssignOpp Expression
					|	Expression BinaryOp Expression
					|	LambdaExpression
					|	PrefixUnaryOp Expression
```
```
FloatLiteral		=>	['0' ... '9']* '.' ('0' ... '9')+ [('e' | 'E') ['+' | '-'] ('0' ... '9')+] ['f' | 'F' | 'h' | 'H' | 'q' | 'Q']
```
```
Function			=>	'extern' 'def' FunctionID Template LambdaExpression
					|	'def' FunctionID Template LambdaExpression
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
IfElse				=>	'if' '(' Expression ')' Expression
					|	'if' '(' Expression ')' Expression 'else' Expression
					|	'if' '(' Expression ')' Expression 'else' IfElse
```
```
IntLiteral			=>	['1' ... '9'] ('0' ... '9')+
					|	'0' ('b' | 'B') ('0' | '1')+
					|	'0' ('o' | 'O') ('0' ... '7')+
					|	'0' ('x' | 'X') ('0' ... '9' | 'a' ... 'f' | 'A' ... 'F')+
```
```
LambdaExpression	=>	'(' Parameters ')' '{' Program '}'
					|	'(' Parameters ')' '->' '(' Parameters ')'
					|	'(' Parameters ')' '->' '(' Parameters ')' '{' Program '}'
```
```
Literal				=>	CharArrayLiteral
					|	FloatLiteral
					|	IntLiteral
					|	StringLiteral
```
```
List				=>	Expression
					|	Expression ',' List
					|	ε
```
```
MetaTags			=>	'@'Atom
					|	'@'Atom MetaTags
```
```
Param				=>	Expression
					|	Variable
```
```
Parameters			=>	MetaTags Param
					|	MetaTags Param ',' Parameters
					|	Param
					|	Param ',' Parameters
					|	ε
```
```
PrefixUnaryOp		=>	'-' | '!' | 'not' | '~'
```
```
Program				=>	Decl Program
					|	ε
```
```
Statement			=>	Assignment
					|	Enum
					|	Expression
					|	Function
					|	Import
					|	List
					|	Struct
					|	Union
					|	Variable
					|	'return' Expression
```
```
StringLiteral		=>	<string double quotes>
```
```
Struct				=>	'struct' Identifier Template '{' Parameters '}'
```
```
Template			=>	TBD
					|	ε
```
```
Variable			=>	Atom Identifier
					|	Atom Identifier ':' Expression
```
```
While				=>	'while' '(' Expression ')' Expression
```