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
#include <parse/tree/operator.hpp>
#include <parse/tree/slice.hpp>
#include <parse/tree/variable.hpp>


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

std::map <string, Operator> operators {
	{" = ", {" = ", 1600, RTL}},
	{" += ", {" +=  ", 1600, RTL}},
	{" -= ", {" -= ", 1600, RTL}},
	{" ~= ", {" ~= ", 1600, RTL}},
	{" *= ", {" *= ", 1600, RTL}},
	{" /= ", {" /= ", 1600, RTL}},
	{" %= ", {" %= ", 1600, RTL}},
	{" <<= ", {" <<= ", 1600, RTL}},
	{" >>= ", {" >>= ", 1600, RTL}},
	{" ^= ", {" ^= ", 1600, RTL}},
	{" &= ", {" &= ", 1600, RTL}},
	{" |= ", {" |= ", 1600, RTL}},
	{" => ", {" => ", 1600, RTL}},
	
	{" <& ", {" <& ", 1580, RTL}},
	{" &> ", {" &> ", 1550, RTL}},
	
	{" -> ", {" -> ", 1520, RTL}},
	
	{" , ", {" , ", 1500, LTR}},
	
	{string (" ") + char (Symbol::OR) + " ", {" or ", 1400, LTR}},
	
	{string (" ") + char (Symbol::AND) + " ", {" and ", 1300, LTR}},
	
	{" != ", {" != ", 1200, LTR}},
	{" == ", {" == ", 1200, LTR}},
	
	{" < ", {" < ", 1100, LTR}},
	{" <= ", {" <= ", 1100, LTR}},
	{" > ", {" > ", 1100, LTR}},
	{" >= ", {" >= ", 1100, LTR}},
	
	{" | ", {" | ", 1000, LTR}},
	
	{" ^ ", {" ^ ", 900, LTR}},
	
	{" & ", {" & ", 800, LTR}},
	
	{" << ", {" << ", 700, LTR}},
	{" >> ", {" >> ", 700, LTR}},
	
	{" + ", {" + ", 600, LTR}},
	{" - ", {" - ", 600, LTR}},
	{" ~ ", {" ~ ", 600, LTR}},
	
	{" * ", {" * ", 500, LTR}},
	{" / ", {" / ", 500, LTR}},
	{" % ", {" % ", 500, LTR}},
	
	{string (" ") + char (Symbol::IN) + " ", {" in ", 450, LTR}},
	
	{string (" ") + char (Symbol::AS) + " ", {" as ", 400, LTR}},
	
	{"++ ", {"++ ", 300, RTL}},
	{"-- ", {"-- ", 300, RTL}},
	{"- ", {"- ", 300, RTL}},
	{"! ", {"! ", 300, RTL}},
	{string () + char (Symbol::NOT) + " ", {"not ", 300, RTL}},
	{"~ ", {"~ ", 300, RTL}},
	
	{" ++", {" ++", 200, LTR}},
	{" --", {" --", 200, LTR}},
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
	enum Flags : uint8_t {NONE=0, EXPRESSION_START=0b1, TERMINATE_STATEMENT=0b10, SEGMENT_BEGIN=0b100, SEGMENT_END=0b1000};
	std::array <SymbolFlags::Flags, __UINT8_MAX__> map = {SymbolFlags::NONE};
	struct SetSymbols {
		SetSymbols (const std::vector <std::pair <Symbol, uint8_t>> & flags) {
			for (auto & p : flags) {
				assert (size_t (p.first) < __UINT8_MAX__);
				map [size_t (p.first)] = Flags (p.second);
			}
		}
		
	};
	SetSymbols __set_valuues__ ({
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
};


struct CodeQueue {
	bool more ();
	numbat::lexer::token peakToken ();
	numbat::lexer::token popToken ();
	Symbol peak ();
	Symbol peak (uint32_t index);
	Match shiftPop (const std::string & seen, std::set <string> accepted);
	Match shiftPop (const std::string & seen, std::set <string> accepted, int precedence);
	void shiftPop ();
	void update (uint32_t n);
	CodeQueue (numbat::lexer::tkstring::const_iterator start, numbat::lexer::tkstring::const_iterator end) : itt (start), end (end) {}
	numbat::lexer::tkstring::const_iterator itt, safeitt, end;
	std::deque <numbat::lexer::tkstring::const_iterator> itts;
	string syms;
};


PTNode errorUnexpectedToken (CodeQueue * queue, const string & expected);

PTNode parseBlock (CodeQueue * queue);
PTNode parseIfElse (CodeQueue * queue);
PTNode parseImport (CodeQueue * queue);
PTNode parseList (CodeQueue * queue, PTNode prev=nullptr);
PTNode parseProgram (CodeQueue * queue);
PTNode parseSlice (CodeQueue * queue, PTNode owner=nullptr);
PTNode parseStatement (CodeQueue * queue);
PTNode parseVariable (CodeQueue * queue, PTNode type);

std::vector <PTNode> parseArguments (CodeQueue * queue);
std::vector <PTNode> parseParameterList (CodeQueue * queue);


void clear (CodeQueue * queue) {
	
	uint32_t scope=0;
	SymbolFlags::Flags flags = SymbolFlags::map [size_t (queue->peak ())];
	while (queue->more () and (scope or not (flags & SymbolFlags::TERMINATE_STATEMENT))) {
		scope += !!(flags & SymbolFlags::SEGMENT_BEGIN) - !!(flags & SymbolFlags::SEGMENT_END);
		std::cerr << "Scope: " << scope << std::endl;
		queue->popToken ();
		flags = SymbolFlags::map [size_t (queue->peak ())];
	}
	
}

PTNode errorUnexpectedToken (CodeQueue * queue, const string & expected) {
	
	numbat::lexer::token tkn = queue->peakToken ();
	clear (queue);
	return new ParseTreeError (tkn.line, 0, "Unexpected token '" + tkn.iden + "', expected " + expected);
	
}


PTNode parse (const string & program) {
	
	numbat::lexer::tkstring prog = numbat::lexer::lex (program);
	return parse (prog.begin (), prog.end ());
	
}

PTNode parse (numbat::lexer::tkstring::const_iterator start, numbat::lexer::tkstring::const_iterator end) {
	
	CodeQueue queue (start, end);
	return parseProgram (&queue);
	
}

PTNode parseAssignment (CodeQueue * queue, PTNode lhs=nullptr) {
	
	if (not lhs) {
		lhs = parseList (queue);
	}
	
	Match opp = queue->shiftPop (" ", allocateOperators);
	
	if (opp.iden == "") return errorUnexpectedToken (queue, "an assignment operation");
	
	PTNode rhs = parseList (queue);
	
	
	if (queue->peak () != Symbol::SEMICOLON) {
		rhs = parseAssignment (queue, rhs);
	}
	
	return new ParseTreeOperator (opp.iden, {lhs, rhs});
	
}

PTNode parseAtom (CodeQueue * queue) {
	
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
			atom = new ParseTreeIdentifier (token.line, 0, token.iden);
			break;
		}
		case Symbol::IF:
			atom = parseIfElse (queue);
			break;
		case Symbol::LITERAL: {
			numbat::lexer::token token = queue->popToken ();
			atom = new ParseTreeLiteral (token.line, 0, token.iden, token.type);
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
			assert (queue->peak () == Symbol::SYMBOL_PARENRHESES_RIGHT);
			queue->shiftPop ();
			break;
		case Symbol::SYMBOL_SQUARE_LEFT:
			atom = parseSlice (queue);
			break;
		default:
			break;
	}
	
	PTNode oldAtom;
	
	do {
		
		oldAtom = atom;
		switch (queue->peak ()) {
			case Symbol::SYMBOL_PARENRHESES_LEFT: {
				// Function call
				queue->shiftPop ();
				atom = new ParseTreeCall (atom, parseArguments (queue));
				assert (queue->peak () == Symbol::SYMBOL_PARENRHESES_RIGHT);
				queue->shiftPop ();
				break;
			}
			case Symbol::SYMBOL_PERIOD:
				// Scope resolution
				return new ParseTreeError (atom->getLine (), atom->getPos (), "Scope resolution not yet implemented");
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
	
	std::vector <PTNode> body;
	while (queue->peak () != Symbol::SYMBOL_BRACE_RIGHT and queue->peak () != Symbol::__NONE__) {
		body.push_back (parseStatement (queue));
		while (queue->peak () == Symbol::SEMICOLON) queue->popToken ();
	}
	
	if (queue->peak () != Symbol::SYMBOL_BRACE_RIGHT) {
		return errorUnexpectedToken (queue, "'}'");
	}
	
	queue->shiftPop ();
	if (body.empty ()) {
		return new ParseTree (0, 0);
	} else {
		return new ParseTree (body);
	}
	
}

PTNode parseExpression (CodeQueue * queue, int precedence=__INT_MAX__, PTNode lhs=nullptr) {
	
	if (not lhs) {
		
		Match opp = queue->shiftPop ("", prefixOperators);
		
		if (opp.iden != "") {
			
			lhs = parseExpression (queue, opp.precedence);
			lhs = new ParseTreeOperator (opp.iden, {lhs});
			
		} else {
				
			lhs = parseAtom (queue);
			
		}
		
	}
	
	auto opp = queue->shiftPop (" ", expressionOperators, precedence);//opp.precedence < precedence or (opp.precedence == precedence and opp.isLHS)
	if (opp.iden != "") {
		
		PTNode rhs = parseExpression (queue, opp.precedence);
		PTNode node = new ParseTreeOperator (opp.iden, {lhs, rhs});
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
	
	bool defExtern = false;
	if (queue->peak () == Symbol::EXTERN) {
		defExtern = true;
		queue->shiftPop ();
	}
	
	// drop def
	queue->shiftPop ();
	
	//TODO: Check for extern
	
	if (queue->peak () != Symbol::IDENTIFIER) {
		return errorUnexpectedToken (queue, "identifier");
	}
	numbat::lexer::token token = queue->popToken ();
	
	//TODO: Parse meta tags
	
	if (queue->peak () != Symbol::SYMBOL_PARENRHESES_LEFT) {
		return errorUnexpectedToken (queue, "'('");
	}
	queue->shiftPop ();
	
	std::vector <PTNode> params = parseParameterList (queue), type;
	
	if (queue->peak () != Symbol::SYMBOL_PARENRHESES_RIGHT) {
		return errorUnexpectedToken (queue, "')'");
	}
	queue->shiftPop ();
	
	if (queue->peak () == Symbol::SYMBOL_MINUS and queue->peak (1) == Symbol::SYMBOL_GREATER) {
		queue->shiftPop ();
		queue->shiftPop ();
		if (queue->peak () != Symbol::SYMBOL_PARENRHESES_LEFT) {
			return errorUnexpectedToken (queue, "'('");
		}
		queue->shiftPop ();
		
		type = parseParameterList (queue);
		
		if (queue->peak () != Symbol::SYMBOL_PARENRHESES_RIGHT) {
			return errorUnexpectedToken (queue, "')'");
		}
		queue->shiftPop ();
	}
	
	PTNode body;
	
	if (not defExtern and not (SymbolFlags::map [size_t (queue->peak ())] & SymbolFlags::TERMINATE_STATEMENT)) {
		body = parseStatement (queue);
	}
	return new Function (token.line, 0, token.iden, params, type, body);
	
}

PTNode parseIfElse (CodeQueue * queue) {
	
	// drop if token
	queue->shiftPop ();
	
	if (queue->peak () != Symbol::SYMBOL_PARENRHESES_LEFT) {
		return errorUnexpectedToken (queue, "'('");
	}
	queue->shiftPop ();
	
	PTNode cond = parseExpression (queue);
	
	if (queue->peak () != Symbol::SYMBOL_PARENRHESES_RIGHT) {
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
	
	// drop import token
	queue->shiftPop ();
	
	std::vector <PTNode> path;
	
	for (;;) {
		
		if (queue->peak () == Symbol::LITERAL) {
			auto tkn = queue->popToken ();
			path.push_back (new ParseTreeLiteral (tkn.line, 0, tkn.iden, tkn.type));
			
		} else if (queue->peak () == Symbol::IDENTIFIER) {
			auto tkn = queue->popToken ();
			path.push_back (new ParseTreeIdentifier (tkn.line, 0, tkn.iden));
			
		} else {
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
			iden = new ParseTreeIdentifier (tkn.line, 0, tkn.iden);
			
		} else {
			return errorUnexpectedToken (queue, "an identifier");
		}
	}
	
	return new ParseTreeImport (new ParseTreeImportPath (path), iden);
	
}

PTNode parseList (CodeQueue * queue, PTNode prev) {
	
	std::vector <PTNode> args;
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
		return new ParseTreeList (args);
	} else {
		return args [0];
	}
	
}

PTNode parseProgram (CodeQueue * queue) {
	
	std::vector <PTNode> body;
	while (queue->more ()) {
		body.push_back (parseStatement (queue));
		while (queue->peak () == Symbol::SEMICOLON) queue->popToken ();
	}
	if (body.empty ()) {
		return new ParseTree (0, 0);
	} else {
		return new ParseTree (body);
	}
	
}

PTNode parseStatement (CodeQueue * queue) {
	
	PTNode lhs=nullptr;
	
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
			//return parseStruct (queue);
		case Symbol::UNION:
			//return parseUnion (queue);
			break;
		case Symbol::REF:
		case Symbol::VAL:
		case Symbol::VAR: {
			numbat::lexer::token token = queue->popToken ();
			lhs = new ParseTreeKeyword (token.line, 0, token.iden);
			break;
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
	
	//Drop '['
	queue->shiftPop ();
	
	if (queue->peak () == Symbol::SYMBOL_SQUARE_RIGHT) {
		queue->shiftPop ();
		if (owner) {
			return new ParseTreeIndex (owner, {});
		}
		return errorUnexpectedToken (queue, "slice");
	}
	
	if (queue->peak (1) == Symbol::IN) {
		
		auto tkn = queue->popToken ();
		queue->shiftPop ();
		PTNode range = parseExpression (queue);
		if (queue->peak () == Symbol::SYMBOL_SQUARE_RIGHT) {
			queue->shiftPop ();
			return new ParseTreeSliceForEach (tkn.line, 0, tkn.iden, range);
		}
		return errorUnexpectedToken (queue, "]");
		
	}
	
	PTNode index = nullptr;
	
	if (queue->peak () != Symbol::COLON) {
		
		index = parseExpression (queue);
		
		if (queue->peak () == Symbol::SYMBOL_SQUARE_RIGHT) {
			
			queue->shiftPop ();
			if (owner) {
				return new ParseTreeIndex (owner, {index});
			}
			return errorUnexpectedToken (queue, "]");
			
		}
		
	}
	
	if (queue->peak () != Symbol::COLON) {
		return errorUnexpectedToken (queue, ":");
	}
	
	queue->shiftPop ();
	PTNode end = nullptr;
	PTNode step = nullptr;
	
	if (queue->peak () != Symbol::COLON) {
		end = parseExpression (queue);
	}
	
	if (queue->peak () == Symbol::COLON) {
		
		queue->shiftPop ();
		if (queue->peak () != Symbol::SYMBOL_SQUARE_RIGHT) {
			step = parseExpression (queue);
		}
		
	}
	
	if (queue->peak () == Symbol::SYMBOL_SQUARE_RIGHT) {
		
		queue->shiftPop ();
		//TODO: give slices a proper location
		PTNode slice = new ParseTreeSlice (0, 0, index, end, step);
		if (owner) {
			return new ParseTreeSliceDecorator (owner, slice);
		}
		return slice;
	}
	
	return errorUnexpectedToken (queue, "]");
	
}

PTNode parseVariable (CodeQueue * queue, PTNode type) {
	
	numbat::lexer::token token = queue->popToken ();
	
	if (queue->peak () == Symbol::COLON) {
		
		queue->popToken ();
		return new ParseTreeVariable (type, new ParseTreeIdentifier (token.line, 0, token.iden), parseList (queue));
		
	} else if (queue->peak () == Symbol::SEMICOLON) {
		
		queue->shiftPop ();
		return new ParseTreeVariable (type, new ParseTreeIdentifier (token.line, 0, token.iden));
		
	} if (SymbolFlags::map [size_t (queue->peak ())] & SymbolFlags::TERMINATE_STATEMENT) {
		
		return new ParseTreeVariable (type, new ParseTreeIdentifier (token.line, 0, token.iden));
		
	}
	
	return errorUnexpectedToken (queue, "':' or end of expression");;
	
}


std::vector <PTNode> parseArguments (CodeQueue * queue) {
	
	Symbol next = queue->peak ();
	if (next == Symbol::SYMBOL_PARENRHESES_RIGHT) return {};
	
	std::vector <PTNode> args;
	do {
		
		if (next == Symbol::IDENTIFIER and queue->peak (1) == Symbol::COLON) {
			args.push_back (new ParseTreeError (0, 0, "Named arguments are not yet supported"));
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

std::vector <PTNode> parseParameterList (CodeQueue * queue) {
	
	Symbol next = queue->peak ();
	if (next == Symbol::SYMBOL_PARENRHESES_RIGHT) return {};
	
	std::vector <PTNode> args;
	for (;;) {
		PTNode atom;
		switch (queue->peak ()) {
			case Symbol::VAL:
			case Symbol::VAR: {
				numbat::lexer::token token = queue->popToken ();
				atom = new ParseTreeKeyword (token.line, 0, token.iden);
				break;
			}
			default:
				atom = parseAtom (queue);
				break;
		}
		
		if (queue->peak () == Symbol::IDENTIFIER) {
			args.push_back (parseVariable (queue, atom));
		} else if (queue->peak () != Symbol::SYMBOL_COMMA and not (SymbolFlags::map [size_t (queue->peak ())] & SymbolFlags::TERMINATE_STATEMENT)) {
			args.push_back (parseExpression (queue, __INT_MAX__, atom));
		} else {
			args.push_back (atom);
		}
		
		if (next == Symbol::SYMBOL_COMMA) {
			queue->shiftPop ();
		} else {
			break;
		}
	}
	return args;
	
}


bool CodeQueue::more() {
	
	if (itts.empty ()) update (32);
	return not itts.empty ();
	
}

Symbol CodeQueue::peak () {
	
	if (itts.empty ()) update (32);
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
	
	if (syms.empty ()) update (32);
	if (syms.empty ()) return *safeitt;
	auto t = itts.front ();
	return *t;
	
}

numbat::lexer::token CodeQueue::popToken() {
	
	if (syms.empty ()) update (32);
	auto t = itts.front ();
	syms = syms.substr (1);
	itts.pop_front ();
	return *t;
	
}

void CodeQueue::shiftPop () {
	
	if (syms.empty ()) update (32);
	syms = syms.substr (1);
	itts.pop_front ();
	
}

Match CodeQueue::shiftPop (const std::string & seen, std::set <string> accepted, int precedence) {
	
	if (syms.size () < 8) update (32);
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
				return {
					opp.precedence, opp.ptn
				};
				
			}
			
			return {-1, ""};
			
		}
		
		match = seen + syms.substr (0, ++index);
		acpItt = accepted.lower_bound (match);
		
	}
	
	return {-1, ""};
	
}

Match CodeQueue::shiftPop (const std::string & seen, std::set <string> accepted) {
	
	return shiftPop (seen, accepted, __INT_MAX__);
	
}

void CodeQueue::update (uint32_t n) {
	
	while (syms.size () < n) {
		
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
			default:
				assert (symbolMap.count (itt->iden));
				sym = symbolMap [itt->iden];
				break;
		}
		syms.push_back (char (sym));
		++itt;
		
	}
	
}