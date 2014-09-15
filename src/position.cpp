#include "../include/position.hpp"


namespace numbat {
using namespace lexer;
namespace parser {


Position nextArg (const Position & pos) {
	return Position (pos.itt, findToken (pos.itt, pos.end, TOKEN::symbol, ","));
}

Position nextBody (const Position & pos) {
	tkitt start = findToken (pos.itt, pos.end, TOKEN::symbol, "{") + 1;
	tkitt end = findToken (start, pos.end, TOKEN::symbol, "}");
	return Position (start, end);
}

Position nextExpression (const Position & pos) {
	return Position (pos.itt, findToken (pos.itt, pos.end, TOKEN::semicolon));
}

Position skipBody(const Position& pos) {
	return ++Position (nextBody (pos).end, pos.end);
}


tkitt findToken (tkitt beg, tkitt end, TOKEN tkn, const string & iden) {
	int brace = 0;
	while (beg < end) {
		if (0 == brace and beg->type == tkn) {
			if ("" == iden) {
				return beg;
			} else if (beg->iden == iden) {
				return beg;
			}
		}
		if (beg->type == TOKEN::symbol) {
			if (beg->iden == "(" or beg->iden == "[" or beg->iden == "{") {
				++brace;
			}
		}
		++beg;
		if (beg->type == TOKEN::symbol) {
			if (beg->iden == ")" or beg->iden == "]" or beg->iden == "}") {
				--brace;
			}
		}
	}
	return beg;
}


};
};