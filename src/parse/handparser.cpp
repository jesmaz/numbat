#include <array>
#include <deque>
#include <parse/handparser.hpp>
#include <parse/tree.hpp>
#include <parse/tree/call.hpp>
#include <parse/tree/error.hpp>
#include <parse/tree/identifier.hpp>
#include <parse/tree/ifelse.hpp>
#include <parse/tree/import.hpp>
#include <parse/tree/index.hpp>
#include <parse/tree/function.hpp>
#include <parse/tree/keyword.hpp>
#include <parse/tree/list.hpp>
#include <parse/tree/literal.hpp>
#include <parse/tree/metaTags.hpp>
#include <parse/tree/operator.hpp>
#include <parse/tree/resolvescope.hpp>
#include <parse/tree/slice.hpp>
#include <parse/tree/struct.hpp>
#include <parse/tree/typemodifier.hpp>
#include <parse/tree/variable.hpp>
#include <utility/report.hpp>
#define __ENABLE__PROFILING__
#include <utility/timer.hpp>


namespace parser {


enum class Symbol : char {
	AND,
	AS,
	ASSIGNMENT,
	ATOM,
	COLON,
	DEF,
	DO,
	ELSE,
	ENUM,
	EXPRESSION,
	EXTERN,
	FOR,
	IDENTIFIER,
	IF,
	IMPORT,
	IN,
	LIST,
	LITERAL,
	NOT,
	OR,
	PROGRAM,
	REF,
	SEMICOLON,
	STATEMENT,
	STRUCT,
	UNION,
	VAL,
	VAR,
	VREF,
	WHILE,
	SYMBOL_ASTERISK='*',
	SYMBOL_AT='@',
	SYMBOL_AMPERSAND='&',
	SYMBOL_BRACE_LEFT='{',
	SYMBOL_BRACE_RIGHT='}',
	SYMBOL_COMMA=',',
	SYMBOL_BANG='!',
	SYMBOL_EQUALS='=',
	SYMBOL_GREATER='>',
	SYMBOL_HIGH_HAT='^',
	SYMBOL_LESS='<',
	SYMBOL_MINUS='-',
	SYMBOL_MODULUS='%',
	SYMBOL_PARENRHESES_LEFT='(',
	SYMBOL_PARENRHESES_RIGHT=')',
	SYMBOL_PERIOD='.',
	SYMBOL_PIPE='|',
	SYMBOL_PLUS='+',
	SYMBOL_SHARP='#',
	SYMBOL_SLASH='/',
	SYMBOL_SQUARE_LEFT='[',
	SYMBOL_SQUARE_RIGHT=']',
	SYMBOL_TIDLE='~',
	__NONE__
};

enum RuleType {NONE=0, LTR=1, RTL=2, ACCUM=4, PTN_Sx=8, PTN_xSx=9, PTN_xS=10};

struct Operator {
	string ptn;
	int precedence;
	RuleType rule;
	PTNode(*ctr)(const string &, std::initializer_list <PTNode>);
};


std::map <string, Symbol> symbolMap {
	{"and", Symbol::AND},
	{"as", Symbol::AS},
	{":", Symbol::COLON},
	{"def", Symbol::DEF},
	{"do", Symbol::DO},
	{"else", Symbol::ELSE},
	{"enum", Symbol::ENUM},
	{"for", Symbol::FOR},
	{"if", Symbol::IF},
	{"import", Symbol::IMPORT},
	{"in", Symbol::IN},
	{"not", Symbol::NOT},
	{"or", Symbol::OR},
	{";", Symbol::SEMICOLON},
	{"ref", Symbol::REF},
	{"struct", Symbol::STRUCT},
	{"union", Symbol::UNION},
	{"val", Symbol::VAL},
	{"var", Symbol::VAR},
	{"vref", Symbol::VREF},
	{"while", Symbol::WHILE},
	{"*", Symbol::SYMBOL_ASTERISK},
	{"@", Symbol::SYMBOL_AT},
	{"&", Symbol::SYMBOL_AMPERSAND},
	{"{", Symbol::SYMBOL_BRACE_LEFT},
	{"}", Symbol::SYMBOL_BRACE_RIGHT},
	{",", Symbol::SYMBOL_COMMA},
	{"!", Symbol::SYMBOL_BANG},
	{"=", Symbol::SYMBOL_EQUALS},
	{">", Symbol::SYMBOL_GREATER},
	{"^", Symbol::SYMBOL_HIGH_HAT},
	{"<", Symbol::SYMBOL_LESS},
	{"-", Symbol::SYMBOL_MINUS},
	{"%", Symbol::SYMBOL_MODULUS},
	{"(", Symbol::SYMBOL_PARENRHESES_LEFT},
	{")", Symbol::SYMBOL_PARENRHESES_RIGHT},
	{".", Symbol::SYMBOL_PERIOD},
	{"|", Symbol::SYMBOL_PIPE},
	{"+", Symbol::SYMBOL_PLUS},
	{"#", Symbol::SYMBOL_SHARP},
	{"/", Symbol::SYMBOL_SLASH},
	{"[", Symbol::SYMBOL_SQUARE_LEFT},
	{"]", Symbol::SYMBOL_SQUARE_RIGHT},
	{"~", Symbol::SYMBOL_TIDLE},
};

template <OPERATION opp>
PTNode operatorFactory (const string & s, std::initializer_list <PTNode> v) {
	return new SpecificOperator <opp> (s, v);
}

std::map <string, Operator> operators {
	{" = ", {" = ", 1600, RTL, operatorFactory <OPERATION::ASSIGN>}},
	{" += ", {" += ", 1600, RTL, operatorFactory <OPERATION::ASSIGN>}},
	{" -= ", {" -= ", 1600, RTL, operatorFactory <OPERATION::ASSIGN>}},
	{" ~= ", {" ~= ", 1600, RTL, operatorFactory <OPERATION::ASSIGN>}},
	{" *= ", {" *= ", 1600, RTL, operatorFactory <OPERATION::ASSIGN>}},
	{" /= ", {" /= ", 1600, RTL, operatorFactory <OPERATION::ASSIGN>}},
	{" %= ", {" %= ", 1600, RTL, operatorFactory <OPERATION::ASSIGN>}},
	{" <<= ", {" <<= ", 1600, RTL, operatorFactory <OPERATION::ASSIGN>}},
	{" >>= ", {" >>= ", 1600, RTL, operatorFactory <OPERATION::ASSIGN>}},
	{" ^= ", {" ^= ", 1600, RTL, operatorFactory <OPERATION::ASSIGN>}},
	{" &= ", {" &= ", 1600, RTL, operatorFactory <OPERATION::ASSIGN>}},
	{" |= ", {" |= ", 1600, RTL, operatorFactory <OPERATION::ASSIGN>}},
	{" => ", {" => ", 1600, RTL, operatorFactory <OPERATION::ASSIGN>}},
	
	{" <& ", {" <& ", 1580, RTL, operatorFactory <OPERATION::NONE>}},
	{" &> ", {" &> ", 1550, RTL, operatorFactory <OPERATION::NONE>}},
	
	{" -> ", {" -> ", 1520, RTL, operatorFactory <OPERATION::NONE>}},
	
	{" , ", {" , ", 1500, LTR, operatorFactory <OPERATION::NONE>}},
	
	{string (" ") + char (Symbol::OR) + " ", {" or ", 1400, LTR, operatorFactory <OPERATION::OR>}},
	
	{string (" ") + char (Symbol::AND) + " ", {" and ", 1300, LTR, operatorFactory <OPERATION::AND>}},
	
	{" != ", {" != ", 1200, LTR, operatorFactory <OPERATION::CMPNE>}},
	{" == ", {" == ", 1200, LTR, operatorFactory <OPERATION::CMPEQ>}},
	
	{" < ", {" < ", 1100, LTR, operatorFactory <OPERATION::CMPLT>}},
	{" <= ", {" <= ", 1100, LTR, operatorFactory <OPERATION::CMPLTE>}},
	{" > ", {" > ", 1100, LTR, operatorFactory <OPERATION::CMPGT>}},
	{" >= ", {" >= ", 1100, LTR, operatorFactory <OPERATION::CMPGTE>}},
	
	{" | ", {" | ", 1000, LTR, operatorFactory <OPERATION::BOR>}},
	
	{" ^ ", {" ^ ", 900, LTR, operatorFactory <OPERATION::BXOR>}},
	
	{" & ", {" & ", 800, LTR, operatorFactory <OPERATION::BAND>}},
	
	{" << ", {" << ", 700, LTR, operatorFactory <OPERATION::NONE>}},
	{" >> ", {" >> ", 700, LTR, operatorFactory <OPERATION::NONE>}},
	
	{" + ", {" + ", 600, LTR, operatorFactory <OPERATION::ADD>}},
	{" - ", {" - ", 600, LTR, operatorFactory <OPERATION::SUB>}},
	{" ~ ", {" ~ ", 600, LTR, operatorFactory <OPERATION::CONCAT>}},
	
	{" * ", {" * ", 500, LTR, operatorFactory <OPERATION::MUL>}},
	{" / ", {" / ", 500, LTR, operatorFactory <OPERATION::DIV>}},
	{" % ", {" % ", 500, LTR, operatorFactory <OPERATION::REM>}},
	
	{string (" ") + char (Symbol::IN) + " ", {" in ", 450, LTR, operatorFactory <OPERATION::IN>}},
	
	{string (" ") + char (Symbol::AS) + " ", {" as ", 400, LTR, operatorFactory <OPERATION::AS>}},
	
	{"- ", {"- ", 300, RTL, operatorFactory <OPERATION::NEG>}},
	{"! ", {"! ", 300, RTL, operatorFactory <OPERATION::LNOT>}},
	{string () + char (Symbol::NOT) + " ", {"not ", 300, RTL, operatorFactory <OPERATION::LNOT>}},
	{"~ ", {"~ ", 300, RTL, operatorFactory <OPERATION::BNOT>}},
	
	{" ++", {" ++", 200, LTR, operatorFactory <OPERATION::NONE>}},
	{" --", {" --", 200, LTR, operatorFactory <OPERATION::NONE>}},
};

std::set <string> oppPrecRangeInc (int min, int max) {
	std::set <string> ret;
	for (auto opp : operators) {
		if (min <= opp.second.precedence and opp.second.precedence <= max) {
			ret.insert (opp.first);
		}
	}
	return ret;
}

std::set <string> allocateOperators = oppPrecRangeInc (1501, 1700);
std::set <string> expressionOperators = oppPrecRangeInc (400, 1499);
std::set <string> postfixOperators = oppPrecRangeInc (200, 200);
std::set <string> prefixOperators = oppPrecRangeInc (300, 300);


namespace SymbolFlags {
	enum Flags : uint8_t {NONE=0, EXPRESSION_START=0b1, TERMINATE_STATEMENT=0b10, SEGMENT_BEGIN=0b100, SEGMENT_END=0b1000, VARIABLE_DEL=0b10000};
	std::array <SymbolFlags::Flags, __UINT8_MAX__> map = {SymbolFlags::NONE};
	struct SetSymbols {
		SetSymbols (const BasicArray <std::pair <Symbol, uint8_t>> & flags) {
			for (auto & p : flags) {
				assert (size_t (p.first) < __UINT8_MAX__);
				map [size_t (p.first)] = Flags (p.second);
			}
		}
		
	};
	SetSymbols __set_valuues__ ({
		{Symbol::COLON,                    VARIABLE_DEL},
		{Symbol::ELSE,                     TERMINATE_STATEMENT},
		{Symbol::IDENTIFIER,               EXPRESSION_START},
		{Symbol::IF,                       EXPRESSION_START},
		{Symbol::LITERAL,                  EXPRESSION_START},
		{Symbol::NOT,                      EXPRESSION_START},
		{Symbol::SEMICOLON,                TERMINATE_STATEMENT},
		{Symbol::WHILE,                    EXPRESSION_START},
		{Symbol::SYMBOL_BANG,              EXPRESSION_START},
		{Symbol::SYMBOL_BRACE_LEFT,        EXPRESSION_START | SEGMENT_BEGIN},
		{Symbol::SYMBOL_BRACE_RIGHT,       TERMINATE_STATEMENT | SEGMENT_END},
		{Symbol::SYMBOL_COMMA,             VARIABLE_DEL},
		{Symbol::SYMBOL_MINUS,             EXPRESSION_START},
		{Symbol::SYMBOL_PARENRHESES_LEFT,  EXPRESSION_START | SEGMENT_BEGIN},
		{Symbol::SYMBOL_PARENRHESES_RIGHT, TERMINATE_STATEMENT | SEGMENT_END},
		{Symbol::SYMBOL_PLUS,              EXPRESSION_START},
		{Symbol::SYMBOL_SQUARE_LEFT,       EXPRESSION_START | SEGMENT_BEGIN},
		{Symbol::SYMBOL_SQUARE_RIGHT,      SEGMENT_END},
		{Symbol::SYMBOL_TIDLE,             EXPRESSION_START},
	});
};


struct Match {
	int precedence;
	string iden;
	PTNode(*ctr)(const string &, std::initializer_list <PTNode>);
};


struct CodeQueue {
	bool more ();
	numbat::lexer::token peakToken ();
	numbat::lexer::token popToken ();
	Symbol peak ();
	Symbol peak (uint32_t index);
	Match shiftPop (const std::string & seen, std::set <string> accepted);
	Match shiftPop (const std::string & seen, std::set <string> accepted, int precedence);
	void markDirty ();
	void shiftPop ();
	void update (uint32_t n=128);
	CodeQueue (numbat::lexer::tkstring::const_iterator start, numbat::lexer::tkstring::const_iterator end, const numbat::File * file) : itt (start), end (end), file (file) {}
	numbat::lexer::tkstring::const_iterator itt, safeitt, end;
	std::deque <numbat::lexer::tkstring::const_iterator> itts;
	string syms;
	const numbat::File * file;
	bool dirty=false;
};

PTNode decorateNodeWithTags (const BasicArray <PTNode> & metaTags, PTNode node);

PTNode errorUnexpectedToken (CodeQueue * queue, const string & expected);

PTNode parseBlock (CodeQueue * queue);
PTNode parseIfElse (CodeQueue * queue);
PTNode parseImport (CodeQueue * queue);
PTNode parseList (CodeQueue * queue, PTNode prev=nullptr);
PTNode parseProgram (CodeQueue * queue);
PTNode parseSlice (CodeQueue * queue, PTNode owner=nullptr);
PTNode parseStatement (CodeQueue * queue);
PTNode parseStruct (CodeQueue * queue);
PTNode parseVariable (CodeQueue * queue);
PTNode parseVariable (CodeQueue * queue, PTNode type);

BasicArray <PTNode> parseArguments (CodeQueue * queue);
BasicArray <PTNode> parseMetaTags (CodeQueue * queue);
BasicArray <PTNode> parseParameterList (CodeQueue * queue);


void clear (CodeQueue * queue) {
	
	uint32_t scope=0;
	SymbolFlags::Flags flags = SymbolFlags::map [size_t (queue->peak ())];
	while (queue->more () and (scope or not (flags & SymbolFlags::TERMINATE_STATEMENT))) {
		scope += !!(flags & SymbolFlags::SEGMENT_BEGIN) - !!(flags & SymbolFlags::SEGMENT_END);
		queue->popToken ();
		flags = SymbolFlags::map [size_t (queue->peak ())];
	}
	
}

PTNode decorateNodeWithTags (const BasicArray <PTNode> & metaTags, PTNode node) {
	return new MetaTags ({metaTags.begin (), metaTags.end ()}, node);
}

PTNode errorUnexpectedToken (CodeQueue * queue, const string & expected) {
	
	queue->markDirty ();
	numbat::lexer::token tkn = queue->peakToken ();
	clear (queue);
	report::logMessage (report::ERROR, queue->file, tkn.pos, "Unexpected token '" + tkn.iden + "', expected " + expected);
	return new ParseTreeError (tkn.pos, "Unexpected token '" + tkn.iden + "', expected " + expected);
	
}


PTNode parse (const string & program, const numbat::File * file) {
	
	numbat::lexer::tkstring prog = numbat::lexer::lex (program);
	return parse (prog.begin (), prog.end (), file);
	
}

PTNode parse (numbat::lexer::tkstring::const_iterator start, numbat::lexer::tkstring::const_iterator end, const numbat::File * file) {
	
	CodeQueue queue (start, end, file);
	return parseProgram (&queue);
	
}

PTNode parseAssignment (CodeQueue * queue, PTNode lhs=nullptr) {
	PROFILE ("parseAssignment");
	if (not lhs) {
		lhs = parseList (queue);
	}
	
	Match opp = queue->shiftPop (" ", allocateOperators);
	
	if (opp.ctr == nullptr) {
		delete lhs;
		return errorUnexpectedToken (queue, "an assignment operation");
	}
	
	PTNode rhs = parseList (queue);
	
	
	if (not (SymbolFlags::map [size_t (queue->peak ())] & SymbolFlags::TERMINATE_STATEMENT)) {
		rhs = parseAssignment (queue, rhs);
	}
	
	return opp.ctr (opp.iden, {lhs, rhs});
	
}

PTNode parseAtom (CodeQueue * queue) {
	PROFILE ("parseAtom");
	PTNode atom = nullptr;
	switch (queue->peak ()) {
		case Symbol::DO:
			//atom = parseDo (queue);
			break;
		case Symbol::FOR:
			//atom = parseFor (queue);
			break;
		case Symbol::IDENTIFIER: {
			numbat::lexer::token token = queue->popToken ();
			atom = new ParseTreeIdentifier (token.pos, token.iden);
			break;
		}
		case Symbol::IF:
			atom = parseIfElse (queue);
			break;
		case Symbol::LITERAL: {
			numbat::lexer::token token = queue->popToken ();
			atom = new ParseTreeLiteral (token.pos, token.iden, token.type);
			break;
		}
		case Symbol::WHILE:
			//atom = parseWhile (queue);
			break;
		case Symbol::SYMBOL_BRACE_LEFT:
			queue->shiftPop ();
			atom = parseBlock (queue);
			break;
		case Symbol::SYMBOL_PARENRHESES_LEFT:
			queue->shiftPop ();
			atom = parseList (queue);
			if (queue->peak () != Symbol::SYMBOL_PARENRHESES_RIGHT) {
				delete atom;
				return errorUnexpectedToken (queue, "')'");
			}
			queue->shiftPop ();
			break;
		case Symbol::SYMBOL_SQUARE_LEFT:
			atom = parseSlice (queue);
			break;
		default:
			break;
	}
	
	if (not atom) {
		return errorUnexpectedToken (queue, "an expression");
	}
	
	PTNode oldAtom;
	
	do {
		
		oldAtom = atom;
		switch (queue->peak ()) {
			case Symbol::SYMBOL_PARENRHESES_LEFT: {
				// Function call
				queue->shiftPop ();
				auto args = parseArguments (queue);
				if (queue->peak () != Symbol::SYMBOL_PARENRHESES_RIGHT) {
					delete atom;
					delAll (args);
					return errorUnexpectedToken (queue, "')'");
				}
				atom = new ParseTreeCall (atom, {args.begin (), args.end ()});
				queue->shiftPop ();
				break;
			}
			case Symbol::SYMBOL_PERIOD: {
				// Scope resolution
				queue->shiftPop ();
				if (queue->peak () != Symbol::IDENTIFIER) {
					delete atom;
					return errorUnexpectedToken (queue, "an identifier");
				}
				numbat::lexer::token token = queue->popToken ();
				atom = new ResolveScope (atom, token.iden);
				break;
			}
			case Symbol::SYMBOL_SQUARE_LEFT:
				// Slice
				atom = parseSlice (queue, atom);
				break;
			default:
				break;
		}
		
	} while (oldAtom != atom);
	
	return atom;
	
}

PTNode parseBlock (CodeQueue * queue) {
	PROFILE ("parseBlock");
	DynArray <PTNode> body;
	while (queue->peak () != Symbol::SYMBOL_BRACE_RIGHT and queue->peak () != Symbol::__NONE__) {
		if (queue->peak () == Symbol::SYMBOL_AT) {
			auto tags = parseMetaTags (queue);
			auto stmt = parseStatement (queue);
			body.push_back (decorateNodeWithTags (tags, stmt));
		} else {
			body.push_back (parseStatement (queue));
		}
		while (
			queue->peak () != Symbol::SYMBOL_BRACE_RIGHT
			and
			SymbolFlags::map [size_t (queue->peak ())] & SymbolFlags::TERMINATE_STATEMENT
		) queue->popToken ();
	}
	
	if (queue->peak () != Symbol::SYMBOL_BRACE_RIGHT) {
		delAll (body);
		return errorUnexpectedToken (queue, "'}'");
	}
	
	queue->shiftPop ();
	if (body.empty ()) {
		return new ParseTree ({0, 0});
	} else {
		return new ParseTree (body);
	}
	
}

PTNode parseExpression (CodeQueue * queue, int precedence=__INT_MAX__, PTNode lhs=nullptr) {
	PROFILE ("parseExpression");
	if (not lhs) {
		
		Match opp = queue->shiftPop ("", prefixOperators);
		
		if (opp.ctr != nullptr) {
			
			lhs = parseExpression (queue, opp.precedence);
			lhs = opp.ctr (opp.iden, {lhs});
			
		} else {
				
			lhs = parseAtom (queue);
			
		}
		
	}
	
	auto opp = queue->shiftPop (" ", expressionOperators, precedence);//opp.precedence < precedence or (opp.precedence == precedence and opp.isLHS)
	if (opp.ctr != nullptr) {
		
		PTNode rhs = parseExpression (queue, opp.precedence);
		PTNode node = opp.ctr (opp.iden, {lhs, rhs});
		if (precedence > opp.precedence) {
			return parseExpression (queue, precedence, node);
		} else {
			return node;
		}
		
	} else {
		
		return lhs;
		
	}
	
}

PTNode parseFunction (CodeQueue * queue) {
	PROFILE ("parseFunction");
	nir::LINKAGE linkage = nir::LINKAGE::LOCAL;
	if (queue->peak () == Symbol::EXTERN) {
		linkage = nir::LINKAGE::EXTERNAL;
		queue->shiftPop ();
	}
	
	if (queue->peak () != Symbol::DEF) {
		return errorUnexpectedToken (queue, "def");
	}
	
	// drop def
	auto pos = queue->popToken ().pos;
	
	string iden = "";
	
	if (queue->peak () != Symbol::SYMBOL_PARENRHESES_LEFT) {
		
		if (queue->peak () != Symbol::IDENTIFIER and queue->peak () != Symbol::LITERAL) {
			return errorUnexpectedToken (queue, "identifier");
		}
		iden = queue->popToken ().iden;
		
	}
	
	if (queue->peak () != Symbol::SYMBOL_PARENRHESES_LEFT) {
		return errorUnexpectedToken (queue, "'('");
	}
	queue->shiftPop ();
	
	BasicArray <PTNode> params = parseParameterList (queue), type;
	
	if (queue->peak () != Symbol::SYMBOL_PARENRHESES_RIGHT) {
		delAll (params);
		return errorUnexpectedToken (queue, "')'");
	}
	queue->shiftPop ();
	
	if (queue->peak () == Symbol::SYMBOL_MINUS and queue->peak (1) == Symbol::SYMBOL_GREATER) {
		queue->shiftPop ();
		queue->shiftPop ();
		if (queue->peak () != Symbol::SYMBOL_PARENRHESES_LEFT) {
			delAll (params);
			return errorUnexpectedToken (queue, "'('");
		}
		queue->shiftPop ();
		
		type = parseParameterList (queue);
		
		if (queue->peak () != Symbol::SYMBOL_PARENRHESES_RIGHT) {
			delAll (params);
			delAll (type);
			return errorUnexpectedToken (queue, "')'");
		}
		queue->shiftPop ();
	}
	
	PTNode body=nullptr;
	
	if (linkage != nir::LINKAGE::EXTERNAL and not (SymbolFlags::map [size_t (queue->peak ())] & SymbolFlags::TERMINATE_STATEMENT)) {
		body = parseStatement (queue);
	}
	return new Function (pos, iden, params, type, body, linkage);
	
}

PTNode parseIfElse (CodeQueue * queue) {
	PROFILE ("parseIfElse");
	// drop if token
	queue->shiftPop ();
	
	if (queue->peak () != Symbol::SYMBOL_PARENRHESES_LEFT) {
		return errorUnexpectedToken (queue, "'('");
	}
	queue->shiftPop ();
	
	PTNode cond = parseExpression (queue);
	
	if (queue->peak () != Symbol::SYMBOL_PARENRHESES_RIGHT) {
		delete cond;
		return errorUnexpectedToken (queue, "')'");
	}
	queue->shiftPop ();
	
	PTNode body = parseStatement (queue);
	PTNode el = nullptr;
	
	if (queue->peak () == Symbol::ELSE) {
		
		queue->shiftPop ();
		
		el = parseStatement (queue);
		
	}
		
	return new ParseTreeIfElse (cond, body, el);
	
}

PTNode parseImport (CodeQueue * queue) {
	PROFILE ("parseImport");
	// drop import token
	auto pos = queue->popToken ().pos;
	
	DynArray <PTNode> path;
	
	for (;;) {
		
		if (queue->peak () == Symbol::LITERAL) {
			auto tkn = queue->popToken ();
			path.push_back (new ParseTreeLiteral (tkn.pos, tkn.iden, tkn.type));
			
		} else if (queue->peak () == Symbol::IDENTIFIER) {
			auto tkn = queue->popToken ();
			path.push_back (new ParseTreeIdentifier (tkn.pos, tkn.iden));
			
		} else {
			delAll (path);
			return errorUnexpectedToken (queue, "a literal or an identifier");
		}
		
		if (queue->peak () == Symbol::SYMBOL_PERIOD) {
			queue->shiftPop ();
		} else {
			break;
		}
		
	}
	
	PTNode iden=nullptr;
	
	if (queue->peak () == Symbol::AS) {
		queue->shiftPop ();
		if (queue->peak () == Symbol::IDENTIFIER) {
			auto tkn = queue->popToken ();
			iden = new ParseTreeIdentifier (tkn.pos, tkn.iden);
			
		} else {
			delAll (path);
			return errorUnexpectedToken (queue, "an identifier");
		}
	}
	
	return new ParseTreeImport (new ParseTreeImportPath ({path.begin (), path.end ()}), iden);
	
}

PTNode parseList (CodeQueue * queue, PTNode prev) {
	PROFILE ("parseList");
	DynArray <PTNode> args;
	if (prev) {
		args.push_back (prev);
	} else {
		args.push_back (parseExpression (queue));
	}
	
	while (queue->peak () == Symbol::SYMBOL_COMMA) {
		queue->shiftPop ();
		args.push_back (parseExpression (queue));
	}
	
	if (args.size () != 1) {
		return new ParseTreeList ({args.begin (), args.end ()});
	} else {
		return args [0];
	}
	
}

PTNode parseProgram (CodeQueue * queue) {
	PROFILE ("parseProgram");
	DynArray <PTNode> body;
	while (queue->more ()) {
		body.push_back (parseStatement (queue));
		while (SymbolFlags::map [size_t (queue->peak ())] & SymbolFlags::TERMINATE_STATEMENT) queue->popToken ();
	}
	if (body.empty ()) {
		return new ParseTree ({0, 0});
	} else {
		return new ParseTree (body);
	}
	
}

PTNode parseStatement (CodeQueue * queue) {
	PROFILE ("parseStatement");
	PTNode lhs=nullptr;
	BasicArray <PTNode> metaTags = parseMetaTags (queue);
	
	switch (queue->peak ()) {
		case Symbol::DEF:
			return parseFunction (queue);
		case Symbol::ENUM:
			//return parseEnum (queue);
		case Symbol::EXTERN:
			return parseFunction (queue);
		case Symbol::IMPORT:
			return parseImport (queue);
		case Symbol::STRUCT:
			return parseStruct (queue);
		case Symbol::UNION:
			//return parseUnion (queue);
			break;
		case Symbol::REF:
		case Symbol::VAL:
		case Symbol::VAR:
		case Symbol::VREF: {
			return parseVariable (queue);
		}
		default:
			break;
	}
	
	if (not lhs) {
		lhs = parseExpression (queue);
	}
	
	
	switch (queue->peak ()) {
		case Symbol::IDENTIFIER:
		case Symbol::SYMBOL_AT:
			return parseVariable (queue, lhs);
		default:
			break;
	}
	
	if (queue->peak () == Symbol::SYMBOL_COMMA) {
		lhs = parseList (queue, lhs);
	}
	
	if (SymbolFlags::map [size_t (queue->peak ())] & SymbolFlags::TERMINATE_STATEMENT) {
		return lhs;
	}
	
	return parseAssignment (queue, lhs);
	
}

PTNode parseSlice (CodeQueue * queue, PTNode owner) {
	PROFILE ("parseSlice");
	//Drop '['
	auto pos = queue->popToken ().pos;
	
	if (queue->peak () == Symbol::IDENTIFIER and queue->peak (1) == Symbol::IN) {
		
		auto tkn = queue->popToken ();
		queue->shiftPop ();
		PTNode range = parseExpression (queue);
		if (queue->peak () == Symbol::SYMBOL_SQUARE_RIGHT) {
			queue->shiftPop ();
			return new ParseTreeSliceForEach (pos, tkn.iden, range);
		}
		
		delete range;
		return errorUnexpectedToken (queue, "]");
		
	}
	
	bool trueSlice=false;
	
	PTNode index = nullptr;
	PTNode end = nullptr;
	PTNode step = nullptr;
	
	if (SymbolFlags::map [size_t (queue->peak ())] & SymbolFlags::EXPRESSION_START) {
		index = parseExpression (queue);
	}
	
	if (queue->peak () == Symbol::COLON) {
		
		trueSlice = true;
		queue->popToken ();
		
		if (SymbolFlags::map [size_t (queue->peak ())] & SymbolFlags::EXPRESSION_START) {
			end = parseExpression (queue);
		}
		
		if (queue->peak () == Symbol::COLON) {
			
			queue->popToken ();
			
			if (SymbolFlags::map [size_t (queue->peak ())] & SymbolFlags::EXPRESSION_START) {
				step = parseExpression (queue);
			}
			
		}
		
	}
	
	if (queue->peak () == Symbol::SYMBOL_SQUARE_RIGHT) {
		
		queue->shiftPop ();
		
		if (trueSlice) {
			
			PTNode slice = new ParseTreeSlice (pos, index, end, step);
			if (owner) {
				return new ParseTreeSliceDecorator (owner, slice);
			}
			return slice;
			
		} else if (owner) {
			if (index) {
				return new ParseTreeIndex (owner, {index});
			} else {
				return new ParseTreeIndex (owner, {});
			}
		}
		
		delete index;
		delete end;
		delete step;
		return errorUnexpectedToken (queue, "slice");
		
	}
	
	delete index;
	delete end;
	delete step;
	return errorUnexpectedToken (queue, "]");
	
}

PTNode parseStruct (CodeQueue * queue) {
	PROFILE ("parseStruct");
	// Drop struct keyword
	auto pos = queue->popToken ().pos;
	
	string iden;
	if (queue->peak () == Symbol::IDENTIFIER) {
		numbat::lexer::token token = queue->popToken ();
		iden = token.iden;
	}
	
	BasicArray <PTNode> members;
	if (queue->peak () == Symbol::SYMBOL_BRACE_LEFT) {
		queue->shiftPop ();
		members = parseParameterList (queue);
		if (queue->peak () != Symbol::SYMBOL_BRACE_RIGHT) {
			delAll (members);
			PTNode err = errorUnexpectedToken (queue, "'}' or ','");
			if (queue->peak () == Symbol::SYMBOL_BRACE_RIGHT) queue->shiftPop ();
			return err;
		}
		queue->shiftPop ();
	}
	
	return new Struct (pos, iden, members);
	
}

PTNode parseVariable (CodeQueue * queue) {
	PROFILE ("parseVariable(2)");
	numbat::lexer::token token = queue->popToken ();
	PTNode type;
	
	if (SymbolFlags::map [size_t (queue->peak (1))] & SymbolFlags::VARIABLE_DEL) {
		type = new ParseTreeKeyword (token.pos, token.iden);
		
	} else if (queue->peak (1) == Symbol::SEMICOLON) {
		return errorUnexpectedToken (queue, "an identifier");
		
	} else {
		type = parseAtom (queue);
		type = new ParseTreeTypeModifier (type, token.pos, token.iden);
		
	}
	
	return parseVariable (queue, type);
	
}

PTNode parseVariable (CodeQueue * queue, PTNode type) {
	PROFILE ("parseVariable(3)");
	numbat::lexer::token token = queue->popToken ();
	
	if (queue->peak () == Symbol::COLON) {
		
		queue->popToken ();
		return new ParseTreeVariable (type, new ParseTreeIdentifier (token.pos, token.iden), parseList (queue));
		
	} else if (queue->peak () == Symbol::SEMICOLON) {
		
		queue->shiftPop ();
		return new ParseTreeVariable (type, new ParseTreeIdentifier (token.pos, token.iden));
		
	} else if (SymbolFlags::map [size_t (queue->peak ())] & SymbolFlags::TERMINATE_STATEMENT) {
		
		return new ParseTreeVariable (type, new ParseTreeIdentifier (token.pos, token.iden));
		
	} else if (queue->peak () == Symbol::SYMBOL_COMMA) {
		
		return new ParseTreeVariable (type, new ParseTreeIdentifier (token.pos, token.iden));
		
	}
	
	delete type;
	return errorUnexpectedToken (queue, "':' or end of expression");;
	
}


BasicArray <PTNode> parseArguments (CodeQueue * queue) {
	PROFILE ("parseArguments");
	Symbol next = queue->peak ();
	if (next == Symbol::SYMBOL_PARENRHESES_RIGHT) return {};
	
	DynArray <PTNode> args;
	do {
		
		if (next == Symbol::IDENTIFIER and queue->peak (1) == Symbol::COLON) {
			args.push_back (new ParseTreeError (queue->peakToken ().pos, "Named arguments are not yet supported"));
		} else {
			args.push_back (parseExpression (queue));
		}
		
		next = queue->peak ();
		if (next == Symbol::SYMBOL_COMMA) {
			queue->shiftPop ();
		} else {
			break;
		}
	} while (true);
	
	return args;
	
}

BasicArray <PTNode> parseMetaTags (CodeQueue * queue) {
	PROFILE ("parseMetaTags");
	DynArray <PTNode> metaTags;
	while (queue->peak () == Symbol::SYMBOL_AT) {
		
		queue->popToken ();
		if (queue->peak () != Symbol::IDENTIFIER) {
			metaTags.push_back (errorUnexpectedToken (queue, "an identifier"));
		} else {
			metaTags.push_back (parseAtom (queue));
		}
		
		if (queue->peak () == Symbol::SEMICOLON) {
			queue->popToken ();
			// This allows for the following:
			// @foo
			// @bar
			// def baz ()
		}
		
	}
	
	return metaTags;
	
}

BasicArray <PTNode> parseParameterList (CodeQueue * queue) {
	PROFILE ("parseParameterList");
	Symbol next = queue->peak ();
	if (next == Symbol::SYMBOL_PARENRHESES_RIGHT or next == Symbol::SYMBOL_BRACE_RIGHT) return {};
	
	DynArray <PTNode> args, metaTags;
	for (;;) {
		PTNode atom;
		switch (queue->peak ()) {
			case Symbol::SYMBOL_AT:
				metaTags = parseMetaTags (queue);
				continue;
			case Symbol::VAL:
			case Symbol::VAR: {
				numbat::lexer::token token = queue->popToken ();
				atom = new ParseTreeKeyword (token.pos, token.iden);
				break;
			}
			default:
				atom = parseAtom (queue);
				break;
		}
		
		if (queue->peak () == Symbol::IDENTIFIER) {
			atom = parseVariable (queue, atom);
		} else if (queue->peak () != Symbol::SYMBOL_COMMA and not (SymbolFlags::map [size_t (queue->peak ())] & SymbolFlags::TERMINATE_STATEMENT)) {
			atom = parseExpression (queue, __INT_MAX__, atom);
		}
		
		if (not metaTags.empty ()) {
			atom = decorateNodeWithTags (metaTags, atom);
			metaTags.clear ();
		}
		args.push_back (atom);
		
		if (queue->peak () == Symbol::SYMBOL_COMMA) {
			queue->shiftPop ();
		} else {
			break;
		}
		
	}
	return args;
	
}


bool CodeQueue::more() {
	
	if (itts.empty ()) update ();
	return not itts.empty ();
	
}

Symbol CodeQueue::peak () {
	
	if (itts.empty ()) update ();
	if (syms.empty ()) {
		return Symbol::__NONE__;
	}
	return Symbol (syms.front ());
	
}

Symbol CodeQueue::peak (uint32_t index) {
	
	if (itts.empty ()) update (32 < index ? 32 : index);
	if (syms.size () <= index) {
		return Symbol::__NONE__;
	}
	return Symbol (syms [index]);
	
}

numbat::lexer::token CodeQueue::peakToken() {
	
	if (syms.empty ()) update ();
	if (syms.empty ()) return *safeitt;
	auto t = itts.front ();
	return *t;
	
}

numbat::lexer::token CodeQueue::popToken() {
	
	if (syms.empty ()) update ();
	auto t = itts.front ();
	syms = syms.substr (1);
	itts.pop_front ();
	dirty = false;
	return *t;
	
}

void CodeQueue::shiftPop () {
	
	if (syms.empty ()) update ();
	syms = syms.substr (1);
	itts.pop_front ();
	dirty = false;
	
}

Match CodeQueue::shiftPop (const std::string & seen, std::set <string> accepted, int precedence) {
	PROFILE ("shiftPop");
	if (syms.size () < 8) update ();
	string match = seen + syms.substr (0, 1);
	uint8_t index=1;
	auto acpItt = accepted.lower_bound (match);
	auto acpEnd = accepted.end ();
	
	while (acpItt != acpEnd and acpItt->compare (0, match.size (), match) == 0 and index < 32) {
		
		if (*acpItt == match or (*acpItt == match + " " and (SymbolFlags::map [syms [index]] & SymbolFlags::EXPRESSION_START))) {
			
			auto opp = operators [*acpItt];
			int prec = opp.precedence;
			if (prec < precedence or (prec == precedence and opp.rule == RTL)) {
				
				syms = syms.substr (match.size ()-seen.size ());
				for (size_t i=0; i<match.size ()-seen.size (); ++i) itts.pop_front ();
				dirty = false;
				return {
					opp.precedence, opp.ptn, opp.ctr
				};
				
			}
			
			return {-1, "", nullptr};
			
		}
		
		match = seen + syms.substr (0, ++index);
		acpItt = accepted.lower_bound (match);
		
	}
	
	return {-1, "", nullptr};
	
}

Match CodeQueue::shiftPop (const std::string & seen, std::set <string> accepted) {
	
	return shiftPop (seen, accepted, __INT_MAX__);
	
}

void CodeQueue::markDirty () {
	if (dirty) {
		if (not syms.empty () or itt != end) {
			shiftPop ();
			dirty = false;
		}
	} else {
		dirty = true;
	}
}

void CodeQueue::update (uint32_t n) {
	PROFILE ("update");
	while (syms.size () < n) {
		
		PROFILE ("update - loop body");
		while (itt != end and (itt->type == numbat::lexer::TOKEN::whitespace or itt->type == numbat::lexer::TOKEN::indent)) ++itt;
		if (itt==end) return;
		
		safeitt = itt;
		itts.push_back (itt);
		Symbol sym;
		switch (itt->type) {
			case numbat::lexer::TOKEN::def:
				sym = Symbol::DEF;
				break;
			case numbat::lexer::TOKEN::externdef:
				sym = Symbol::EXTERN;
				break;
			case numbat::lexer::TOKEN::identifier:
				sym = Symbol::IDENTIFIER;
				break;
			case numbat::lexer::TOKEN::import:
				sym = Symbol::IMPORT;
				break;
			case numbat::lexer::TOKEN::chararrayliteral:
			case numbat::lexer::TOKEN::numericliteral:
			case numbat::lexer::TOKEN::stringliteral:
				sym = Symbol::LITERAL;
				break;
			case numbat::lexer::TOKEN::semicolon:
				sym = Symbol::SEMICOLON;
				break;
			default: {
				PROFILE ("update - loop body - symb lookup");
				auto sym_itt = symbolMap.find (itt->iden);
				if (sym_itt != symbolMap.end ()) {
					sym = sym_itt->second;;
				} else {
					PROFILE ("update - loop body - symb lookup - log");
					report::logMessage (report::ERROR, file, itt->pos, "Unidentified symbol '" + itt->iden + "'");
					sym = Symbol::__NONE__;
				}
				break;
			}
		}
		syms.push_back (char (sym));
		++itt;
		
	}
	
}


}
