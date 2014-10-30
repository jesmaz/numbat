#include "../include/core.hpp"
#include "../include/parser.hpp"
#include "../include/ast/astidentifier.hpp"

using namespace numbat::lexer;
using namespace numbat::parser;

void addOperator (ParsingContext * context, int precidence, bool ltr, const string & pattern, OperatorDecleration::OperatorParser parser, OperatorDecleration::DefaultImplementation defImp = nullptr) {
	
	OperatorDecleration oppdec (precidence, ltr, pattern, nullptr, parser, defImp);
	
	if (context->operators.find (pattern) != context->operators.end ()) {
		return;
	}
	
	shared_ptr <OperatorDecleration> opp (new OperatorDecleration (oppdec));
	
	context->operators [pattern] = opp;
	
	for (const string & s : opp->getSymbols ()) {
		if (s != " ") {
			context->operatorsByFirstToken.insert (std::make_pair (s.substr(0,1), opp));
			break;
		}
	}
	for (const string & s : opp->getSymbols ()) {
		if (s != " ") {
			context->keywords.insert(s);
		}
	}
	
}

void addBlock (ParsingContext * context, const string & open, const string & close) {
	context->blocks [open] = close;
}

AbstractSyntaxTree * ast = new AbstractSyntaxTree ();
ParsingContext * context = getContext (ast);

void parseTest (const string & code) {
	auto program = lex (code);
	Position pos (program.begin (), findToken (program.begin (), program.end (), TOKEN::semicolon));
	std::cout << parseExpression (pos, ast)->toString () << std::endl;
}

void parseBodyTest (const string & code) {
	auto program = lex (code);
	Position pos (program.begin (), findToken (program.begin (), program.end (), TOKEN::eof));
	std::cout << parseBody (pos, ast)->toString () << std::endl;
}

int main () {
	
	addBlock (context, "(", ")");
	addBlock (context, "[", "]");
	addBlock (context, "{", "}");
// 	addBlock (context, "?", ":");
	
	addOperator (context, 100, true, "{}", parseBlockOperator);
	addOperator (context, 100, true, "{ }", parseBlockOperator);
	addOperator (context, 100, true, "( )", parseSubExpression);
	addOperator (context, 100, true, " ( )", parseCall);
	addOperator (context, 100, true, " ()", parseCall);
	//addOperator (context, 100, true, "[ ] ");
	addOperator (context, 100, true, " [ ] ", parseArrayDecleration);
	addOperator (context, 100, true, " []", parseIndex);
	addOperator (context, 100, true, " [ ]", parseIndex);
	
	addOperator (context, 300, false, "- ", parseUnary, defNegation);
	addOperator (context, 300, false, "! ", parseUnary, defNegation);
	
	addOperator (context, 500, true, " + ", parseBinary, defArithmetic);
	addOperator (context, 500, true, " - ", parseBinary, defArithmetic);
// 	addOperator (context, 500, true, " ~ ", parseBinary, defConcat);
	
	//addOperator (context, 1100, true, " or ", parseBinary, defLogic);
	
	addOperator (context, 1400, true, " , ", parseComma);
	
	//addOperator (context, 1500, true, " ? : ", parseTernaryOperator);
	
	addOperator (context, 1600, false, " = ", parseAssignmentOperator);
	
	//addOperator (context, 1700, false, "if( ) ");
	//addOperator (context, 1700, false, "if( ) else ");
	//addOperator (context, 1700, false, "lock( ) ");
	addOperator (context, 1700, false, "while( ) ", parseWhileLoop);
	
	if (!createRawType (ast, "int32", 32, NumbatRawType::SIGNED)) return 1;
	if (!createRawType (ast, "uint64", 64, NumbatRawType::UNSIGNED)) return 1;
	if (!createRawType (ast, "double", 64, NumbatRawType::FLOAT)) return 1;
	ASTnode intType (new ASTtype (0, false, false, getType (ast, "int32")));
	for (char c = 'a'; c <= 'z'; ++c) {
		createVariable (ast, intType, nullptr, (string () + c), false, false);
	}
	
	parseTest ("3 + 5");
	parseTest ("3.0 + 5");
	parseTest ("3 + 5.0");
	parseTest ("3.0 + 5.0");
	parseTest ("a = 3");
	parseTest ("a = 3 + 5");
	parseTest ("a = b - 3 + 5");
	parseTest ("a = b = c + 2 - 5 + 49");
	parseTest ("n = 4 + 6 + g");
	parseTest ("n = 4 + -6 + g");
	parseTest ("n = 4 + (6 + g)");
// 	parseTest ("n = a ? b : c");
// 	parseTest ("n = a ? b = f : c");
// 	parseTest ("n = 7 + (a - 8 ? b - 5 + 8 : 8 + c) ? 0 : 9");
// 	parseTest ("n = 7 + a - 8 ? b - 5 + 8 : 8 + c ? 0 : 9");
	parseTest ("n = b (r)");
	parseTest ("n = b ()");
	parseTest ("n = k ( )");
	parseTest ("n = k + y (8 -9) + 50");
	parseTest ("n = !k");
	parseTest ("n = !k + !5 ()");
	parseTest ("n, m = a, b");
	parseTest ("n, m = a, b = d, e");
	parseTest ("n, m = a, b + 5, 6");
	parseTest ("n, m = a, b = !e, g + 6, 7");
	parseTest ("n, m = a, b + y");
// 	parseTest ("n, m = a, b ? c, d : e, f");
// 	parseTest ("n, m = a, b ? c, d + 7, 6 - 3 : e, f");
	parseTest ("n = f (x, y - 1, z)");
	//parseTest ("[n] r [10] t");
	parseBodyTest ("a = b; r = f; t = d + 4\n t = y - 9\n r = c ? a : b\n g = y");
	parseTest ("{a + b}");
	//parseTest ("a or b");
	//parseTest ("a or b");
	parseTest ("while (a) c");
	parseTest ("while(b) (a - h)");
	parseTest ("while(b) {}");
	parseTest ("while(b) {a - h}");
// 	parseTest ("if(a)b");
// 	parseTest ("if(a){b}");
// 	parseTest ("if(a){b}else{c}");
// 	parseTest ("e = if (a) b else c");
// 	parseTest ("if(a){\nb\n}else{\nc\n}");
// 	parseTest ("if(a){\nb\n}else if (e) {\nc\n} else if (g) {\nf\n}");
// 	parseTest ("x = if (a) b else c");
// 	parseTest ("x = if (a) b = y else c = z");
// 	parseTest ("x = if (a) b = y else z + u");
// 	parseTest ("lock (a) b");
// 	parseTest ("lock (a) {b + c}");
// 	parseTest ("lock (a) while (b) {}");
	parseTest ("int32 A");
	parseTest ("int32 B = a");
	parseTest ("int32 C = a + b - c");
	parseTest ("a = a + int32 F - c");
	parseTest ("j = F");
	parseBodyTest (
		"r = func (n)\n"
		"struct testType {int32 first}\n"
		"def func (int32 arg, int32 arg2, ) -> (int32 ret) {\n"
		"F = arg\n"
		"}\n"
		"r = func (n)"
	);
	parseBodyTest (
		"r = func (n)\n"
		"struct testType {int32 first}\n"
		"def func (int32 arg, int32 arg2, ) -> (int32 ret) {\n"
		"while (r) {\n"
		"a = b\n"
		"while (r) {\n"
		"r = 6 + d\n"
		"}\n"
		"}\n"
		"F = arg\n"
		"}\n"
		"r = func (n)"
	);
	parseTest ("{\n\tint32 r = f;\n}");
	
	std::cout << "\n\n\n\n" << ast->NumbatScope::toString ();
	
	delete ast;
	return 0;
	
}