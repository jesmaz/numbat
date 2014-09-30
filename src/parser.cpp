#include "../include/ast/astidentifier.hpp"
#include "../include/core.hpp"
#include "../include/parser.hpp"

namespace numbat {
using namespace lexer;
namespace parser {


ASTnode generateError (const Position & pos, const string & message) {
	std::ostringstream ss;
	ss << pos.itt->line << ": " << message;
	return ASTnode (new ASTerror (ss.str ()));
}

ASTnode generateOperatorError (const Position & pos, const string & message) {
	std::ostringstream ss;
	ss << pos.itt->line << ": " << message;
	return ASTnode (new ASToperatorError (ss.str ()));
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

ASTnode parseNumericliteral (const Position & pos, NumbatScope * scope) {
	
	const string & str = pos.itt->iden;
	ASTnode num;
	const NumbatType * ftype = nullptr;
	if (str.back () == 'f') {
		ftype = getType (scope, "float");
	} else if (str.back () == 'h') {
		ftype = getType (scope, "half");
	} else if (str.back () == 'q') {
		ftype = getType (scope, "quad");
	} else if (str.find ('.') != string::npos) {
		ftype = getType (scope, "double");
	} else {
		size_t l = std::stoull (str);
		ftype = getType (scope, "uint64");
		if (ftype) {
			num = ASTnode (new ASTconstantInt (ASTnode (new ASTtype (false, true, ftype)), l));
		}
	}
	
	if (!num) {
		if (ftype) {
			num = ASTnode (new ASTconstantFPInt (ASTnode (new ASTtype (false, true, ftype)), str));
		} else {
			num = generateError (pos, "There is no type available for this kind of literal");
		}
	}
	
	if (Position p = pos + 1) {
		return generateOperatorError (pos+1, "Unexpected token: '" + (pos+1).itt->iden + "'");
	}
	return num;
	
}

ASTnode parsePrimaryExpression (Position pos, NumbatScope * scope) {
	
	ASTnode symb = nullptr;
	switch (pos.itt->type) {
		case TOKEN::chararrayliteral:
			return ASTnode (new ASTerror ("NYI"));
			break;
		case TOKEN::nil:
			return ASTnode (new ASTnil ());
			break;
		case TOKEN::numericliteral:
			return parseNumericliteral (pos, scope);
			break;
		case TOKEN::stringliteral:
			if (pos+1) return generateOperatorError (pos+1, "Unexpected token: '" + (pos+1).itt->iden + "'");
			return ASTnode (new ASTconstantCString (ASTnode (new ASTtype (false, true, getType (scope, "string"))), parseString (pos)));
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
				symb = ASTnode (new ASTvariable (var));
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
		return ASTnode (new ASTtype (ref, constTkn, type));
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
		
		auto oppBeg = context->operatorsByFirstToken.lower_bound (pos.itt->iden.substr (0, 1));
		auto oppEnd = context->operatorsByFirstToken.upper_bound (pos.itt->iden.substr (0, 1));
		for (; oppBeg != oppEnd; ++oppBeg) {
			OperatorDecleration::OperatorMatch match;
			match.opp = oppBeg->second;
			match.ptr = pos.itt;
			match.level = brace;
			candidates.push_back (std::make_pair (match, oppBeg->second->getPattern ().find_first_not_of (" ")));
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


void * futureFunc (void * data) {
	
	std::tuple <Position, NumbatScope *, FunctionDecleration *> * args = reinterpret_cast <std::tuple <Position, NumbatScope *, FunctionDecleration *> *> (data);
	Position body = nextBody (std::get <0> (*args));
	std::get <2> (*args)->assignBody (parseBody (body ? body : std::get <0> (*args), std::get <1> (*args)));
	delete args;
	return nullptr;
	
}

void * futureStruct (void * data) {
	
	std::tuple <Position, NumbatScope *, NumbatType *> * args = reinterpret_cast <std::tuple <Position, NumbatScope *, NumbatType *> *> (data);
	std::get <2> (*args)->buildData (parseArgs (std::get <0> (*args), std::get <1> (*args)));
	delete args;
	return nullptr;
	
}


};
};