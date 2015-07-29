#include "../../../include/ast/constant/astconstantcstring.hpp"
#include "../../../include/ast/constant/astconstantint.hpp"
#include "../../../include/ast/constant/astconstantfpint.hpp"
#include "../../../include/parse/tree/literal.hpp"

numbat::parser::ASTnode ParseTreeLiteral::build (numbat::parser::NumbatScope * scope) {
	switch (tokenType) {
		case numbat::lexer::TOKEN::chararrayliteral: {
			numbat::parser::ASTnode type (new numbat::parser::ASTtype (getLine (), false, true, numbat::parser::getType (scope, "string")));
			return numbat::parser::ASTnode (new numbat::parser::ASTconstantCString (getLine (), type, literal));
		}
			
		case numbat::lexer::TOKEN::numericliteral: {
			string fStr;
			numbat::parser::ASTnode num;
			const numbat::parser::NumbatType * ftype = nullptr;
			if (literal.back () == 'f') {
				ftype = numbat::parser::getType (scope, "float");
				fStr = literal.substr (0, literal.length ()-1);
			} else if (literal.back () == 'h') {
				ftype = numbat::parser::getType (scope, "half");
				fStr = literal.substr (0, literal.length ()-1);
			} else if (literal.back () == 'q') {
				ftype = numbat::parser::getType (scope, "quad");
				fStr = literal.substr (0, literal.length ()-1);
			} else if (literal.find ('.') != string::npos) {
				ftype = numbat::parser::getType (scope, "double");
				fStr = literal;
			} else {
				size_t l = std::stoull (literal);
				ftype = numbat::parser::getType (scope, "uint64");
				if (ftype) {
					num = numbat::parser::ASTnode (new numbat::parser::ASTconstantInt (getLine (), numbat::parser::ASTnode (new numbat::parser::ASTtype (getLine (), false, true, ftype)), l));
				}
			}
			if (!num) {
				if (ftype) {
					num = numbat::parser::ASTnode (new numbat::parser::ASTconstantFPInt (getLine (), numbat::parser::ASTnode (new numbat::parser::ASTtype (getLine (), false, true, ftype)), fStr));
				} else {
					//num = numbat::parser::ASTnode (new numbat::parser::ASTerror (getLine (), "No type could be found for numerical literal"));
					std::cerr << "No type could be found for numerical literal" << std::endl;
				}
			}
			return num;
		}
			
		case numbat::lexer::TOKEN::stringliteral: {
			numbat::parser::ASTnode type (new numbat::parser::ASTtype (getLine (), false, true, numbat::parser::getType (scope, "string")));
			return numbat::parser::ASTnode (new numbat::parser::ASTconstantCString (getLine (), type, literal));
		}
		
		default:
			std::cerr << "Invalid token type" << std::endl;
			return nullptr;
	}
}

string ParseTreeLiteral::strDump (text::PrintMode mode) {
	return mode & text::COLOUR ? text::yel + literal + text::reset : literal;
}