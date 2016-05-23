#include <deque>

#include <parse/tree.hpp>
#include <parse/tree/error.hpp>
#include <parse/tree/identifier.hpp>
#include <parse/tree/list.hpp>
#include <parse/tree/literal.hpp>
#include <parse/tree/operator.hpp>
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
	FOR,
	IDENTIFIER,
	IF,
	IN,
	LIST,
	LITERAL,
	NOT,
	OR,
	PROGRAM,
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
	SYMBOL_HIGH_HAT='^',
	SYMBOL_LESS='<',
	SYMBOL_MINUS='-',
	SYMBOL_PARENRHESES_LEFT='(',
	SYMBOL_PARENRHESES_RIGHT=')',
	SYMBOL_PLUS='+',
	SYMBOL_SHARP='#',
	SYMBOL_SQUARE_LEFT='[',
	SYMBOL_SQUARE_RIGHT=']',
	__NONE__
};

enum RuleType {NONE=0, LTR=1, RTL=2, ACCUM=4, PTN_Sx=8, PTN_xSx=9, PTN_xS=10};

struct Operator {
	string ptn;
	int precidance;
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
	{"not", Symbol::NOT},
	{"or", Symbol::OR},
	{";", Symbol::SEMICOLON},
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
	{"^", Symbol::SYMBOL_HIGH_HAT},
	{"<", Symbol::SYMBOL_LESS},
	{"-", Symbol::SYMBOL_MINUS},
	{"(", Symbol::SYMBOL_PARENRHESES_LEFT},
	{")", Symbol::SYMBOL_PARENRHESES_RIGHT},
	{"+", Symbol::SYMBOL_PLUS},
	{"#", Symbol::SYMBOL_SHARP},
	{"[", Symbol::SYMBOL_SQUARE_LEFT},
	{"]", Symbol::SYMBOL_SQUARE_RIGHT},
};

std::map <string, Operator> operators {
	{"=", {" = ", 1600, RTL}},
	{"+=", {" += ", 1600, RTL}},
	{"-=", {" -= ", 1600, RTL}},
	{"~=", {" ~= ", 1600, RTL}},
	{"*=", {" *= ", 1600, RTL}},
	{"/=", {" /= ", 1600, RTL}},
	{"%=", {" %= ", 1600, RTL}},
	{"<<=", {" <<= ", 1600, RTL}},
	{">>=", {" >>= ", 1600, RTL}},
	{"^=", {" ^= ", 1600, RTL}},
	{"&=", {" &= ", 1600, RTL}},
	{"|=", {" |= ", 1600, RTL}},
	{"=>", {" => ", 1600, RTL}},
	
	{string () + char (Symbol::OR), {" or ", 1400, LTR}},
	
	{string () + char (Symbol::AND), {" and ", 1300, LTR}},
	
	{"!=", {" != ", 1200, LTR}},
	{"==", {" == ", 1200, LTR}},
	
	{"<", {" < ", 1100, LTR}},
	{"<=", {" <= ", 1100, LTR}},
	{">", {" > ", 1100, LTR}},
	{">=", {" >= ", 1100, LTR}},
	
	{"|", {" | ", 1000, LTR}},
	
	{"^", {" ^ ", 900, LTR}},
	
	{"&", {" & ", 800, LTR}},
	
	{"<<", {" << ", 700, LTR}},
	{">>", {" >> ", 700, LTR}},
	
	{"+", {" + ", 600, LTR}},
	{"-", {" - ", 600, LTR}},
	{"~", {" ~ ", 600, LTR}},
	
	{"*", {" * ", 500, LTR}},
	{"/", {" / ", 500, LTR}},
	{"%", {" % ", 500, LTR}},
	
	{string () + char (Symbol::IN), {" in ", 450, LTR}},
	
	{string () + char (Symbol::AS), {" as ", 400, LTR}},
	
	{"++", {"++ ", 300, RTL}},
	{"--", {"-- ", 300, RTL}},
	{"-", {"- ", 300, RTL}},
	{"!", {"! ", 300, RTL}},
	{string () + char (Symbol::NOT), {"not ", 300, RTL}},
	{"~", {"~ ", 300, RTL}},
	
	{"++", {" ++", 200, LTR}},
	{"--", {" --", 200, LTR}},
};

std::set <string> oppPrecRangeInc (int min, int max) {
	std::set <string> ret;
	for (auto opp : operators) {
		if (min <= opp.second.precidance and opp.second.precidance <= max) {
			ret.insert (opp.first);
		}
	}
	return ret;
}

std::set <string> allocateOperators = oppPrecRangeInc (1500, 1700);
std::set <string> expressionOperators = oppPrecRangeInc (400, 1500);
std::set <string> postfixOperators = oppPrecRangeInc (200, 200);
std::set <string> prefixOperators = oppPrecRangeInc (300, 300);


struct Match {
	int precidance;
	string iden;
};


struct CodeQueue {
	bool more ();
	numbat::lexer::token popToken ();
	Symbol peak ();
	Match shiftPop (std::set <string> accepted);
	Match shiftPop (std::set <string> accepted, int precidance);
	void shiftPop ();
	void update (uint32_t n);
	CodeQueue (numbat::lexer::tkstring::const_iterator start, numbat::lexer::tkstring::const_iterator end) : itt (start), end (end) {}
	numbat::lexer::tkstring::const_iterator itt, end;
	std::deque <numbat::lexer::tkstring::const_iterator> itts;
	string syms;
};


PTNode parse (numbat::lexer::tkstring::const_iterator start, numbat::lexer::tkstring::const_iterator end);
PTNode parseBlock (CodeQueue * queue);
PTNode parseList (CodeQueue * queue, PTNode prev=nullptr);
PTNode parseProgram (CodeQueue * queue);
PTNode parseStatment (CodeQueue * queue);
PTNode parseVariable (CodeQueue * queue, PTNode type);


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
	
	Match opp = queue->shiftPop (allocateOperators);
	
	if (opp.iden == "") return new ParseTreeError (lhs->getLine (), lhs->getPos (), "Expected an assignment operation");
	
	PTNode rhs = parseList (queue);
	
	
	if (queue->peak () != Symbol::SEMICOLON) {
		rhs = parseAssignment (queue, rhs);
	}
	
	return new ParseTreeOperator (opp.iden, {lhs});
	
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
			//atom = parseIf (queue);
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
			break;
		default:
			break;
	}
	
	Match opp = queue->shiftPop ({"."});
	
	if (opp.iden != "") {
		return new ParseTreeError (atom->getLine (), atom->getPos (), "Scope resolution not yet implemented");
	}
	return atom;
	
}

PTNode parseBlock (CodeQueue * queue) {
	
	std::vector <PTNode> body;
	while (queue->peak () != Symbol::SYMBOL_BRACE_RIGHT and queue->peak () != Symbol::__NONE__) {
		body.push_back (parseStatment (queue));
		while (queue->peak () == Symbol::SEMICOLON) queue->popToken ();
	}
	
	if (queue->peak () != Symbol::SYMBOL_BRACE_RIGHT) {
		//TODO: make queue aware of it's position
		return new ParseTreeError (0, 0, "Expected a }");
	}
	
	queue->shiftPop ();
	if (body.empty ()) {
		return new ParseTree (0, 0);
	} else {
		return new ParseTree (body);
	}
	
}

PTNode parseExpression (CodeQueue * queue, int precidance=__INT_MAX__, PTNode lhs=nullptr) {
	
	if (not lhs) {
		
		//TODO: replace with appropriate set of unary
		Match opp = queue->shiftPop (prefixOperators);
		
		if (opp.iden != "") {
			
			lhs = parseExpression (queue, opp.precidance);
			lhs = new ParseTreeOperator (opp.iden, {lhs});
			
		} else {
			
			lhs = parseAtom (queue);
			
		}
		
	}
	
	auto opp = queue->shiftPop (expressionOperators, precidance);//opp.precidance < precidance or (opp.precidance == precidance and opp.isLHS)
	if (opp.iden != "") {
		
		PTNode rhs = parseExpression (queue, opp.precidance);
		PTNode node = new ParseTreeOperator (opp.iden, {lhs, rhs});
		if (precidance > opp.precidance) {
			return parseExpression (queue, precidance, node);
		} else {
			return node;
		}
		
	} else {
		
		return lhs;
		
	}
	
}

PTNode parseList (CodeQueue * queue, PTNode prev) {
	
	std::vector <PTNode> args;
	if (prev) args.push_back (prev);
	
	args.push_back (parseExpression (queue));
	while (queue->peak () == Symbol::SYMBOL_COMMA) {
		queue->shiftPop ();
		args.push_back (parseExpression (queue));
	}
	
	return new ParseTreeList (args);
	
}

PTNode parseProgram (CodeQueue * queue) {
	
	std::vector <PTNode> body;
	while (queue->more ()) {
		body.push_back (parseStatment (queue));
		while (queue->peak () == Symbol::SEMICOLON) queue->popToken ();
	}
	if (body.empty ()) {
		return new ParseTree (0, 0);
	} else {
		return new ParseTree (body);
	}
	
}

PTNode parseStatment (CodeQueue * queue) {
	
	PTNode lhs=nullptr;
	
	switch (queue->peak ()) {
		case Symbol::DEF:
			//return parseFunction (queue);
		case Symbol::ENUM:
			//return parseEnum (queue);
		case Symbol::STRUCT:
			//return parseStruct (queue);
		case Symbol::UNION:
			//return parseUnion (queue);
			break;
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
			return parseVariable (queue, lhs);
		default:
			break;
	}
	
	if (queue->peak () == Symbol::SYMBOL_COMMA) {
		lhs = parseList (queue, lhs);
	}
	
	if (queue->peak () == Symbol::SEMICOLON) {
		queue->shiftPop ();
		return lhs;
	}
	
	if (queue->peak () == Symbol::SYMBOL_BRACE_RIGHT) return lhs;
	
	return parseAssignment (queue, lhs);
	
}

PTNode parseVariable (CodeQueue * queue, PTNode type) {
	
	numbat::lexer::token token = queue->popToken ();
	
	if (queue->peak () == Symbol::COLON) {
		
		queue->popToken ();
		return new ParseTreeVariable (type, new ParseTreeIdentifier (token.line, 0, token.iden), parseList (queue));
		
	} else if (queue->peak () == Symbol::SEMICOLON) {
		
		queue->shiftPop ();
		return new ParseTreeVariable (type, new ParseTreeIdentifier (token.line, 0, token.iden));
		
	}
	
	//TODO: make a helper function for unexpected tokens
	return new ParseTreeError (0, 0, "Unexpected token");
	
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

numbat::lexer::token CodeQueue::popToken() {
	
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

Match CodeQueue::shiftPop (std::set <string> accepted, int precidance) {
	
	if (syms.size () < 8) update (32);
	string match = syms.substr (0, 1);
	auto acpItt = accepted.lower_bound (match);
	auto acpEnd = accepted.end ();
	
	while (acpItt != acpEnd and acpItt->compare (0, match.size (), match) == 0) {
		
		if (*acpItt == match) {
			syms = syms.substr (match.size ());
			for (size_t i=0; i<match.size (); ++i) itts.pop_front ();
			
			int prec = operators [match].precidance;
			if (prec < precidance or (prec == precidance and operators [match].rule == RTL)) {
				return {
					operators [match].precidance, operators [match].ptn
				};
			}
			
			return {-1, ""};
			
		}
		
	}
	
	return {-1, ""};
	
}

Match CodeQueue::shiftPop (std::set <string> accepted) {
	
	return shiftPop (accepted, __INT_MAX__);
	
}

void CodeQueue::update (uint32_t n) {
	
	while (syms.size () < n) {
		
		while (itt != end and (itt->type == numbat::lexer::TOKEN::whitespace or itt->type == numbat::lexer::TOKEN::indent)) ++itt;
		if (itt==end) return;
		
		itts.push_back (itt);
		Symbol sym;
		switch (itt->type) {
			case numbat::lexer::TOKEN::identifier:
				sym = Symbol::IDENTIFIER;
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