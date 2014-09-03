#ifndef OPERATORDECLERATION_HPP
#define OPERATORDECLERATION_HPP

#include "astbase.hpp"
#include "../lexer.hpp"

#include <list>


namespace numbat {
namespace parser {

struct AbstractSyntaxTree;
typedef lexer::tkstring::const_iterator tkitt;

struct OperatorDecleration {
	public:
		enum class TYPE {array, binary, index_call, ternary, unarypostfix, unaryprefix, INVALID};
		
		struct OperatorMatch {
			const bool operator == (const OperatorMatch & rhs) const {return opp == rhs.opp and ptr == rhs.ptr;}
			const static bool treeOrder (const OperatorMatch & lhs, const OperatorMatch & rhs);
			const static bool parseOrder (const OperatorMatch & lhs, const OperatorMatch & rhs) {return lhs.level == rhs.level ? lhs.ptr < rhs.ptr : lhs.level < rhs.level;}
			std::shared_ptr <OperatorDecleration> opp;
			tkitt ptr;
			std::vector <tkitt> ptrs;
			int level;
		};
		
		ASTnode parse (AbstractSyntaxTree * ast, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end) const {return parser (ast, pattern, oppLoc, matches, end);}
		
		const bool isLtr () const {return ltr;}
		const bool operator < (const OperatorDecleration & rhs) const {if (precidance == rhs.precidance) return pattern < rhs.pattern; return precidance < rhs.precidance;}
		const std::vector <string> & getSymbols () const {return symbols;}
		const string & getPattern () const {return pattern;}
		const TYPE getType () const {return type;}
		
		static TYPE calculateOperatorType (const string & pattern);
		
		OperatorDecleration ();
		OperatorDecleration (int precidance, bool ltr, const string & pattern, ASTnode(*parser)(AbstractSyntaxTree *, const string &, const std::vector <tkitt> &, std::list <OperatorDecleration::OperatorMatch> &, tkitt));
		
	private:
		int precidance;
		bool ltr;
		std::vector <string> symbols;
		string pattern; // space for identifier eg: " * ", " [ ]", " ? : "
		TYPE type;
		ASTnode(*parser)(AbstractSyntaxTree *, const string &, const std::vector <tkitt> &, std::list <OperatorDecleration::OperatorMatch> &, tkitt);
};


}
}


#endif /*OPERATORDECLERATION_HPP*/