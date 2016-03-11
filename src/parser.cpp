#include "../include/ast/asterror.hpp"
#include "../include/ast/astidentifier.hpp"
#include "../include/ast/astmodule.hpp"
#include "../include/ast/astnil.hpp"
#include "../include/ast/astoperatorerror.hpp"
#include "../include/ast/control/astreturn.hpp"
#include "../include/ast/constant/astconstantcstring.hpp"
#include "../include/ast/constant/astconstantfpint.hpp"
#include "../include/ast/constant/astconstantint.hpp"
#include "../include/ast/memory/aststructindex.hpp"
#include "../include/core.hpp"
#include "../include/parser.hpp"

namespace numbat {
using namespace lexer;
namespace parser {


ASTnode generateError (const Position & pos, const string & message) {
	return ASTnode (new ASTerror (pos.itt->line, message));
}

ASTnode generateOperatorError (const Position & pos, const string & message) {
	return ASTnode (new ASToperatorError (pos.itt->line, message));
}


ASTnode parseBody (Position pos, NumbatScope * scope) {
	NumbatScope * body = createChild (scope);
	parseBodyInline (pos, body);
	//The memory leak here is on purpose
	return * new ASTnode (body);
}

ASTnode parseExpression (Position pos, NumbatScope * scope) {
	auto matches = generateOperatorMatches (getContext (scope), pos);
	matches.sort (&OperatorDecleration::OperatorMatch::treeOrder);
	return parseExpression (pos, scope, matches);
}

ASTnode parseExpression (Position pos, NumbatScope * scope, std::list <OperatorDecleration::OperatorMatch> matches) {
	
	matches.remove_if ([&pos](const OperatorDecleration::OperatorMatch & arg){return pos.itt > arg.ptrs.front() or arg.ptrs.back() >= pos.end;});
	if (matches.size () == 0) return parsePrimaryExpression (pos, scope);
	
	matches.sort (&OperatorDecleration::OperatorMatch::treeOrder);
	
	OperatorDecleration::OperatorMatch match;
	ASTnode node = nullptr;
	
	for (auto & match : matches) {
		node = parseOperator (pos, scope, *match.opp, &matches, match.ptrs);
		if (node and node->isParsed ()) {
			break;
		}
		
	}
	
	if (node == nullptr) {
		return parsePrimaryExpression (pos, scope);
	}
	
	return node;
}

ASTnode parseNumericliteral (const Position & pos, NumbatScope * scope) {
	
	const string & str = pos.itt->iden;
	string fStr;
	ASTnode num;
	const NumbatType * ftype = nullptr;
	if (str.back () == 'f') {
		ftype = getType (scope, "float");
		fStr = str.substr (0, str.length ()-1);
	} else if (str.back () == 'h') {
		ftype = getType (scope, "half");
		fStr = str.substr (0, str.length ()-1);
	} else if (str.back () == 'q') {
		ftype = getType (scope, "quad");
		fStr = str.substr (0, str.length ()-1);
	} else if (str.find ('.') != string::npos) {
		ftype = getType (scope, "double");
		fStr = str;
	} else {
		size_t l = std::stoull (str);
		ftype = getType (scope, "uint64");
		if (ftype) {
			num = ASTnode (new ASTconstantInt (pos.itt->line, ASTnode (new ASTtype (pos.itt->line, false, true, ftype)), l));
		}
	}
	
	if (!num) {
		if (ftype) {
			num = ASTnode (new ASTconstantFPInt (pos.itt->line, ASTnode (new ASTtype (pos.itt->line, false, true, ftype)), fStr));
		} else {
			num = generateError (pos, "There is no type available for this kind of literal");
		}
	}
	
	if (Position p = pos + 1) {
		return generateOperatorError (pos+1, "Unexpected token: '" + (pos+1).itt->iden + "'");
	}
	return num;
	
}

ASTnode parseOperator (Position pos, NumbatScope * scope, const OperatorDecleration & opp, std::list <OperatorDecleration::OperatorMatch> * matches, std::vector <tkitt> matchPtr) {
	
	std::vector <Position> args;
	const std::vector <string> & symb = opp.getSymbols ();
	
	if (symb.size () < 1) {
		return generateOperatorError (pos, "Invalid operator");
	}
	
	if (symb.front() != " ") {
		if (matchPtr.front () != pos.itt) return generateOperatorError (pos, "Unexpected token '" + pos.itt->iden + "' when parsing '" + opp.getPattern () + "'");
	}
	if (symb.back() != " ") {
		if (++Position (matchPtr.back (), pos.end)) return generateOperatorError (pos, "Unexpected token '" + (++Position (matchPtr.back (), pos.end)).itt->iden  + "' when parsing '" + opp.getPattern () + "'");
	}
	
	Position lhs = pos;
	for (tkitt itt : matchPtr) {
		lhs = Position (pos.itt, itt);
		pos = Position (itt + 1, pos.end);
		if (lhs) {
			args.push_back (lhs);
		}
	}
	if (pos) {
		args.push_back (pos);
	}
	
	size_t arg=0, match=0 ;
	for (const string & s : opp.getSymbols ()) {
		if (s == " ") {
			if (args.size () <= arg) {
				return generateOperatorError (pos, "Not enough arguments");
			}
			++arg;
		} else {
			if (matchPtr.size () <= match) return generateError (Position (matchPtr.front ()), "Syntax error when parsing '" + opp.getPattern () + "' operator");
			if (args.size () > arg) {
				if (!(matchPtr [match] < args [arg])) {
					return generateOperatorError (pos, "Argument error (probably a bug) when parsing '" + opp.getPattern () + "'");
				}
			}
			++match;
		}
	}
	
	//matches->sort (&OperatorDecleration::OperatorMatch::parseOrder);
	
	if (opp.parsable ()) return opp.parse (scope, opp.getPattern (), args, matches);
	
	//TODO: return opp.parse (scope, args, matches);
	string err;// = " (";
	arg=0;
	for (const string & s : opp.getSymbols ()) {
		if (s == " ") {
			ASTnode exp = parseExpression (args [arg], scope, *matches);
			//if (!exp->isValid()) return exp;
			if (typeid (*exp.get ()) == typeid (ASToperatorError)) return exp;
			err += exp->toString ();
			++arg;
		} else {
			err += " " + s + " ";
		}
	}
	return ASTnode (new ASTidentifier (/*opp.getSymbols ().back () == ")" ? err :*/ " (" + err + ") "));
}

ASTnode parsePrimaryExpression (Position pos, NumbatScope * scope) {
	
	ASTnode symb = nullptr;
	switch (pos.itt->type) {
		case TOKEN::chararrayliteral:
			return ASTnode (new ASTerror (pos.itt->line, "NYI"));
			break;
		case TOKEN::nil:
			return ASTnode (new ASTnil (pos.itt->line));
			break;
		case TOKEN::numericliteral:
			return parseNumericliteral (pos, scope);
			break;
		case TOKEN::stringliteral:
			if (pos+1) return generateOperatorError (pos+1, "Unexpected token: '" + (pos+1).itt->iden + "'");
			return ASTnode (new ASTconstantCString (pos.itt->line, ASTnode (new ASTtype (pos.itt->line, false, true, getType (scope, "string"))), parseString (pos)));
			break;
		case TOKEN::symbol:
			return generateOperatorError (pos, "Unexpected symbol: '" + pos.itt->iden + "'");
			break;
		case TOKEN::typemodifier:
			symb = parseType (&pos, scope);
			break;
		default:
			break;
	}
	
	if (!symb) {
		symb = resolveSymbol (scope, pos.itt->iden);
	}
	
	if (typeid (*symb.get ()) == typeid (ASTtype)) {
		if (++pos) {
			NumbatVariable * var = createVariable (scope, symb, nullptr, pos.itt->iden, false, false);
			if (var) {
				symb = ASTnode (new ASTvariable (pos.itt->line, var));
			} else {
				symb = generateError (pos, "'" + pos.itt->iden + "' already declared in this scope");
			}
		}
	}
	if (!symb->isValid ()) return symb;
	if ((pos+1) and (pos+1).itt->type != TOKEN::semicolon) {
		return generateOperatorError (pos+1, "Unexpected token: '" + (pos+1).itt->iden+ "'");
	}
	return symb;
	
}

ASTnode parseType (Position * pos, NumbatScope * scope) {
	
	bool ref=false, constTkn=false;
	size_t line = pos->itt->line;
	while (*pos and pos->itt->type == TOKEN::typemodifier) {
		if (pos->itt->iden == "ref") {
			ref = true;
		} else if (pos->itt->iden == "const") {
			constTkn = true;
		}
		++*pos;
	}
	
	if (*pos) {
		const NumbatType * type = getType (scope, pos->itt->iden);
		if (!type) {
			return generateError (*pos, "'" + pos->itt->iden + "' is not a type");
		}
		return ASTnode (new ASTtype (line, ref, constTkn, type));
	}
	return generateError (*pos, "Expected identifier");
	
}


std::list <OperatorDecleration::OperatorMatch> generateOperatorMatches (const ParsingContext * context, Position pos) {
	
	std::list <OperatorDecleration::OperatorMatch> matches;
	int brace = 0;
	std::list <std::pair <OperatorDecleration::OperatorMatch, size_t>> candidates;
	std::list <std::pair <OperatorDecleration::OperatorMatch, size_t> *> remove;
	std::stack <string> bracket;
	while (pos) {
		
		if (pos.itt->type != TOKEN::stringliteral and pos.itt->type != TOKEN::chararrayliteral) {
			if (!bracket.empty () and (bracket.top () == pos.itt->iden)) {
				bracket.pop ();
				--brace;
			}
			if (0 > brace) break;
		}
		
		if (brace == 0) {
			auto oppBeg = context->operatorsByFirstToken.lower_bound (pos.itt->iden.substr (0, 1));
			auto oppEnd = context->operatorsByFirstToken.upper_bound (pos.itt->iden.substr (0, 1));
			for (; oppBeg != oppEnd; ++oppBeg) {
				OperatorDecleration::OperatorMatch match;
				match.opp = oppBeg->second;
				match.ptr = pos.itt;
				match.level = brace;
				candidates.push_back (std::make_pair (match, oppBeg->second->getPattern ().find_first_not_of (" ")));
			}
		}
		for (auto & cand : candidates) {
			if (brace < cand.first.level) {
				remove.push_back (&cand);
				continue;
			} else if (brace > cand.first.level) {
				continue;
			}
			const string & ptn = cand.first.opp->getPattern ();
			const string & sym = pos.itt->iden;
			
			if (ptn [cand.second] == ' ') {
				size_t len = std::min (ptn.size () - cand.second - 1, sym.size ());
				int cmp = sym.compare (0, len, ptn, cand.second + 1,  len);
				if (!cmp) {
					cand.first.ptrs.push_back (pos.itt);
					cand.second += len + 1;
				}
			} else if (pos.itt->type != TOKEN::chararrayliteral and pos.itt->type != TOKEN::stringliteral) {
				size_t len = std::min (ptn.size () - cand.second, sym.size ());
				int cmp = sym.compare (0, len, ptn, cand.second, len);
				if (cmp) {
					remove.push_back (&cand);
					continue;
				} else {
					cand.first.ptrs.push_back (pos.itt);
					cand.second += len;
				}
			} else {
				remove.push_back (&cand);
				continue;
			}
			if (cand.second >= ptn.size () or (cand.second + 1 >= ptn.size () and ptn.back () == ' ')) {
				matches.push_back (cand.first);
				remove.push_back (&cand);
			}
		}
		for (auto & r : remove) {
			candidates.remove (*r);
		}
		remove.clear ();
		if (pos.itt->type != TOKEN::stringliteral and pos.itt->type != TOKEN::chararrayliteral) {
			auto itt = context->blocks.find (pos.itt->iden);
			if (itt != context->blocks.end ()) {
				bracket.push (itt->second);
				++brace;
			}
		}
		++pos;
	}
	return matches;
	
}


std::vector <ASTnode> parseArgs (Position pos, NumbatScope * scope) {
	std::vector <ASTnode> args;
	while (Position exp = nextArg (pos)) {
		args.push_back (parseExpression (exp, scope));
		pos += exp;
	}
	return args;
}


string parseString (const Position & pos) {
	
	string str;
	bool escaped = false;
	for (char c : pos.itt->iden) {
		if (escaped) {
			switch (c) {
				case 'a':
					str += '\a';
					break;
				case 'b':
					str += '\b';
					break;
				case 'f':
					str += '\f';
					break;
				case 'n':
					str += '\n';
					break;
				case 'r':
					str += '\r';
					break;
				case 't':
					str += '\t';
					break;
				case 'v':
					str += '\v';
					break;
				case '\\':
					str += '\\';
					break;
				case '\'':
					str += '\'';
					break;
				case '\"':
					str += '\"';
					break;
				case '?':
					str += '?';
					break;
			}
			escaped = false;
		} else if (c == '\\') {
			escaped = true;
		} else {
			str += c;
		}
	}
	return str;
	
}


void importModule (NumbatScope * scope, shared_ptr <Module> module, bool extention) {
	scope->getAST ()->importModule (module, extention);
}

void parseBodyInline (Position pos, NumbatScope * scope) {
	
	Position oldPos = pos;
	std::list <std::pair <void*(*)(void*), void*>> futureBody, futureType;
	
	while (Position exp = nextExpression (pos)) {
		/*for (tkitt itt = exp.itt; itt != exp.end; ++itt)
			std::cout << itt->iden;
		std::cout << std::endl;*/
		switch (exp.itt->type) {
			case TOKEN::def:
				if (void * data = parseFunctionDecleration (exp, scope)) {
					futureBody.push_back (std::make_pair (futureFunc, data));
				}
				break;
				
			case TOKEN::enumtkn:
				if (void * data = parseEnumDecleration (exp, scope)) {
					futureType.push_back (std::make_pair (futureEnum, data));
				}
				break;
				
			case TOKEN::externdef:
				parseFunctionDecleration (exp, scope);
				break;
				
			case TOKEN::import:
				parseImport (exp, scope);
				break;
				
			case TOKEN::structure:
				if (void * data = parseStructDecleration (exp, scope)) {
					futureType.push_back (std::make_pair (futureStruct, data));
				}
				break;
				
			case TOKEN::typedeftkn:
				//parseTypeDef (exp, body);
				break;
				
			default:
				break;
		}
		pos += exp;
	}
	
	
	for (auto e : futureType) {
		e.first (e.second);
	}
	
	
	pos = oldPos;
	
	while (Position exp = nextExpression (pos)) {
		switch (exp.itt->type) {
			case TOKEN::def:
			case TOKEN::enumtkn:
			case TOKEN::externdef:
			case TOKEN::import:
			case TOKEN::structure:
			case TOKEN::typedeftkn:
				break;
				
			case TOKEN::ret:
				if (++exp) {
					size_t line = exp.itt->line;
					ASTnode lhs = getFunction (scope)->getType () [0];
					addToBody (scope, ASTnode (new ASTreturn (line, defAssign (scope, " = ", {lhs, parseExpression (exp, scope)}))));
				} else {
					addToBody (scope, ASTnode (new ASTreturnvoid (exp.itt->line)));
				}
				break;
				
			default:
				addToBody (scope, parseExpression (exp, scope));
				break;
		}
		pos += exp;
	}
	
	for (auto e : futureBody) {
		e.first (e.second);
	}
	
}

void parseImport (Position pos, NumbatScope * scope) {
	
	++pos;//eat import token
	
	string mod = pos.itt->iden;
	string iden = mod;
	while (++pos and pos.itt->type == TOKEN::symbol and pos.itt->iden == ".") {
		mod = joinPaths (mod, iden = (++pos).itt->iden);
	}
	
	if (pos.itt->type == TOKEN::as) {
		iden = (++pos).itt->iden;
	}
	
	auto module = Module::import (getContext (scope)->path, mod);
	
	ASTnode type = ASTnode (new ASTmodule (module));
	createVariable (scope, type, nullptr, iden, false, false);
	
}

void parseModule (Position pos, NumbatScope * scope) {
	importModule (scope, Module::createEmpty ("numbat core"), true);
	importModule (scope, Module::import (getContext (scope)->path, "core util"), false);
	
	parseBodyInline (pos, scope);
}

void * futureEnum (void * data) {
	
	std::tuple <Position, NumbatScope *, NumbatEnumType *> * params = reinterpret_cast <std::tuple <Position, NumbatScope *, NumbatEnumType *> *> (data);
	Position pos = std::get <0> (*params);
	NumbatScope * scope = std::get <1> (*params);
	NumbatEnumType * enumType = std::get <2> (*params);
	std::vector <ASTnode> args;
	ASTnode type (new ASTtype (pos.itt->line, false, false, enumType->getBaseType ()));
	ASTnode val (new ASTvariable (pos.itt->line, createVariable (scope, type, nullptr, "init value", true, true)));
	while (Position exp = nextArg (pos)) {
		//args.push_back (parseExpression (exp, scope));
		string iden = exp.itt->iden;
		++exp;
		//TODO: string (regex?) initialisers
		if (exp.itt->type == TOKEN::symbol and exp.itt->iden == "=") {
			++exp;
			val = createStaticCast (parseExpression (exp, scope), type);
		}
		val = ASTnode (new ASTvariable (pos.itt->line, createVariable (scope, type, val, iden, true, false)));
		args.push_back (val);
		pos += exp;
		if (val->getType ()) {
			auto methods = val->getType ()->getMethods ("++ ");
			if (!methods.empty ()) {
				val = findBestMatch ({val}, methods);
			} else {
				ASTnode num;
				if (val->getType ()->isFloat ()) {
					num = ASTnode (new ASTconstantFPInt (pos.itt->line, ASTnode (new ASTtype (pos.itt->line, false, true, val->getType ())), "1.0"));
				} else {
					num = ASTnode (new ASTconstantInt (pos.itt->line, ASTnode (new ASTtype (pos.itt->line, false, true, val->getType ())), 1));
				}
				auto methods = val->getType ()->getMethods (" + ");
				if (!methods.empty ()) {
					val = findBestMatch ({val, num}, methods);
				} else {
					val = defArithmetic (scope, " + ", {val, num});
				}
			}
		}
	}
	enumType->buildData (args);
	
}

void * futureFunc (void * data) {
	
	std::tuple <Position, NumbatScope *, FunctionDecleration *> * args = reinterpret_cast <std::tuple <Position, NumbatScope *, FunctionDecleration *> *> (data);
	Position body = nextBody (std::get <0> (*args));
	ASTnode b = parseBody (body ? body : std::get <0> (*args), std::get <1> (*args));
	if (!b->isReturned ()) {
		if (std::get <2> (*args)->getType ().empty ()) {
			b = ASTnode (new ASTbody (body.end->line, {b, ASTnode (new ASTreturnvoid (body.end->line))}));
		} else {
			b = ASTnode (new ASTreturn (body.end->line, defAssign (std::get <1> (*args), " = ", {getFunction (std::get <1> (*args))->getType () [0], b})));
		}
	}
	std::get <2> (*args)->assignBody (b);
	delete args;
	return nullptr;
	
}

void * futureStruct (void * data) {
	
	std::tuple <Position, NumbatScope *, NumbatType *> * args = reinterpret_cast <std::tuple <Position, NumbatScope *, NumbatType *> *> (data);
	std::get <2> (*args)->buildData (parseArgs (std::get <0> (*args), std::get <1> (*args)));
	delete args;
	return nullptr;
	
}

void * parseEnumDecleration (Position pos, NumbatScope * scope) {
	
	++pos;//eat enum token
	if (pos.itt->type != TOKEN::identifier) {
		std::cout << "Not an identifier" << std::endl;
		std::cout << "'" << pos.itt->iden << "'" << std::endl;
		return nullptr;
	}
	const string & iden = pos.itt->iden;
	std::set <string> tags;
	while (++pos and pos.itt->type == TOKEN::atsym) {
		tags.insert ((++pos).itt->iden);
	}
	const NumbatType * type;
	if (pos.itt->type == TOKEN::colon) {
		++pos;
		Position primPos (pos.itt, findToken (pos.itt, pos.end, TOKEN::symbol, "{"));
		ASTnode primary = parseExpression (primPos, scope);
		type = primary->getType ();
		pos.itt = primPos.end;
	} else {
		type = getType (scope, "uint32");
	}
	if (pos.itt->type != TOKEN::symbol or pos.itt->iden != "{") {
		std::cout << "Expected a '{'" << std::endl;
		std::cout << "'" << pos.itt->iden << "'" << std::endl;
		return nullptr;
	}
	tkitt end = findToken (pos.itt, pos.end, TOKEN::symbol, "}");
	
	NumbatEnumType * enumType = createEnum (scope, iden, type, tags);
	if (enumType) {
		return (void *) new std::tuple <Position, NumbatScope *, NumbatEnumType *> (Position (pos.itt + 1, end), createChild (scope), enumType);
	} else {
		std::cout << "Failed to register" << std::endl;
		std::cout << "'" << pos.itt->iden << "'" << std::endl;
		return nullptr;
	}
	
}

void * parseFunctionDecleration (Position pos, NumbatScope * scope) {
	
	bool future=true;
	if (pos.itt->type == TOKEN::externdef){
		++pos;
		future=false;
	}
	++pos;//eat def token
	if (pos.itt->type != TOKEN::identifier and pos.itt->type != TOKEN::chararrayliteral) {
		std::cout << "Not identifier or char array literal" << std::endl;
		std::cout << "'" << pos.itt->iden << "'" << std::endl;
		return nullptr;
	}
	const string & iden = pos.itt->iden;
	std::set <string> tags;
	while (++pos and pos.itt->type == TOKEN::atsym) {
		tags.insert ((++pos).itt->iden);
	}
	
	if (pos.itt->type != TOKEN::symbol or pos.itt->iden != "(") {
		std::cout << "Missing first '('" << std::endl;
		std::cout << "'" << pos.itt->iden << "'" << std::endl;
		return nullptr;
	}
	
	tkitt end = findToken (pos.itt, pos.end, TOKEN::symbol, ")");
	NumbatScope * fScope = createChild (scope);
	std::vector <ASTnode> param = parseArgs (Position (pos.itt + 1, end), fScope);
	pos.itt = end;
	std::vector <ASTnode> type;
	if ((pos + 1).itt->type == TOKEN::symbol and (pos + 1).itt->iden == "-" and ((pos + 1) + 1).itt->type == TOKEN::symbol and ((pos + 1) + 1).itt->iden == ">") {
		++pos;++pos;
		if ((++pos).itt->type != TOKEN::symbol or pos.itt->iden != "(") {
			std::cout << "Missing second '('" << std::endl;
			std::cout << "'" << pos.itt->iden << "'" << std::endl;
			return nullptr;
		}
		end = findToken (pos.itt, pos.end, TOKEN::symbol, ")");
		type = parseArgs (Position (pos.itt + 1, end), fScope);
		size_t index = 0;
		for (ASTnode & e : type) {
			if (typeid (*e.get ()) != typeid (ASTvariable)) {
				NumbatVariable * var = createVariable (fScope, e, nullptr, iden + " " + e->getIden () + " " + std::to_string (index), false, false);
				e = ASTnode (new ASTvariable (e->getLineNo (), var));
			}
			++index;
		}
		pos.itt = end;
		++pos;
	}
	
	FunctionDecleration * funcDec = createFunctionDecleration (scope, iden, param, type, tags, fScope);
	setFunction (fScope, funcDec);
	if (funcDec) {
		std::set <const NumbatType *> types;
		for (ASTnode & a : param) {
			types.insert (a->getType ());
		}
		for (const NumbatType * type : types) {
			const_cast <NumbatType *> (type)->addMethod (iden, funcDec);
		}
		if (const NumbatType * type = getType (scope, iden)) {
			const_cast <NumbatType *> (type)->addMethod (iden, funcDec);
		}
		if (future) {
			return (void *) new std::tuple <Position, NumbatScope *, FunctionDecleration *> (pos, fScope, funcDec);
		} else {
			return nullptr;
		}
	} else {
		std::cout << "Failed to register" << std::endl;
		return nullptr;
	}
	
}

void * parseStructDecleration (Position pos, NumbatScope * scope) {
	
	++pos;//eat struct token
	if (pos.itt->type != TOKEN::identifier) {
		std::cout << "Not an identifier" << std::endl;
		std::cout << "'" << pos.itt->iden << "'" << std::endl;
		return nullptr;
	}
	const string & iden = pos.itt->iden;
	std::set <string> tags;
	while (++pos and pos.itt->type == TOKEN::atsym) {
		tags.insert ((++pos).itt->iden);
	}
	if (pos.itt->type != TOKEN::symbol or pos.itt->iden != "{") {
		std::cout << "Expected a '{'" << std::endl;
		std::cout << "'" << pos.itt->iden << "'" << std::endl;
		return nullptr;
	}
	tkitt end = findToken (pos.itt, pos.end, TOKEN::symbol, "}");
	
	NumbatType * type = createStruct (scope, iden, tags);
	if (type) {
		return (void *) new std::tuple <Position, NumbatScope *, NumbatType *> (Position (pos.itt + 1, end), createChild (scope), type);
	} else {
		std::cout << "Failed to register" << std::endl;
		std::cout << "'" << pos.itt->iden << "'" << std::endl;
		return nullptr;
	}
	
}


};
};