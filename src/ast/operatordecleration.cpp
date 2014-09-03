#include "../include/ast/operatordecleration.hpp"


namespace numbat {
namespace parser {


const bool OperatorDecleration::OperatorMatch::treeOrder (const OperatorMatch & lhs, const OperatorMatch & rhs) {
	if (lhs.level == rhs.level) {
		if (lhs.opp->precidance == rhs.opp->precidance) {
			if (lhs.opp->ltr) {
				return lhs.ptr > rhs.ptr;
			} else {
				return lhs.ptr < rhs.ptr;
			}
		} else {
			return lhs.opp->precidance > rhs.opp->precidance;
		}
	} else {
		return lhs.level < rhs.level;
	}
}

OperatorDecleration::TYPE OperatorDecleration::calculateOperatorType (const string & pattern) {
	TYPE type = TYPE::INVALID;
	string simplePtn;
	for (char c : pattern) {
		if (c == ' ') {
			if (!(simplePtn.size () and simplePtn.back () == ' '))
				simplePtn += c;
		} else if (!(simplePtn.size () and simplePtn.back () == '*')) {
			simplePtn += '*';
		}
	}
	//std::cout << "'" << simplePtn << "'    '" << pattern << "'" << std::endl;
	switch (simplePtn.length ()) {
		case 2:
			if (simplePtn == " *") type = TYPE::unarypostfix;
			else if (simplePtn == "* ") type = TYPE::unaryprefix;
			break;
		case 3:
			if (simplePtn == " * ") type = TYPE::binary;
			else if (simplePtn == "* *") type = TYPE::array;
			break;
		case 4:
			if (simplePtn == " * *") type = TYPE::index_call;
			break;
		case 5:
			if (simplePtn == " * * ") type = TYPE::ternary;
	}
	return type;
}

OperatorDecleration::OperatorDecleration (int precidance, bool ltr, const string & pattern, ASTnode(*parser)(AbstractSyntaxTree *, const string &, const std::vector <tkitt> &, std::list <OperatorDecleration::OperatorMatch> &, tkitt)) : precidance (precidance), ltr (ltr), pattern (pattern), type (calculateOperatorType (pattern)), parser (parser) {
	string buffer = "";
	for (char c : pattern) {
		if (buffer == "") {
			buffer += c;
		} else {
			if (c == ' ') {
				if (buffer != " ") {
					symbols.push_back (buffer);
					buffer = " ";
				}
			} else {
				if (buffer == " ") {
					symbols.push_back (buffer);
					buffer = c;
				}
			}
		}
	}
	if (buffer != "")
		symbols.push_back (buffer);
}



}
}