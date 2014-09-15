#include "../include/core.hpp"
#include "../include/parser.hpp"

namespace numbat {
using namespace lexer;
namespace parser {


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
		num = ASTnode (new ASTconstantInt (ASTnode (new ASTtype (false, true, getType (scope, "uint64"))), l));
	}
	
	if (ftype) {
		num = ASTnode (new ASTconstantFPInt (ASTnode (new ASTtype (false, true, ftype)), str));
	}
	
	if (Position p = pos + 1) {
		return ASTnode (new ASToperatorError ("Unexpected token: '" + (pos+1).itt->iden + "'"));
	}
	return num;
	
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
	
	
	
}


};
};