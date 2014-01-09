#ifndef OPERATORDECLERATION_HPP
#define OPERATORDECLERATION_HPP

#include "astbase.hpp"
#include "../lexer.hpp"


namespace numbat {
namespace parser {

typedef lexer::tkstring::const_iterator tkitt;

struct OperatorDecleration {
	public:
		enum class TYPE {array, binary, index_call, ternary, unarypostfix, unaryprefix, INVALID};
		
		struct OperatorMatch {
			const static bool treeOrder (const OperatorMatch & lhs, const OperatorMatch & rhs);
			const static bool parseOrder (const OperatorMatch & lhs, const OperatorMatch & rhs) {return lhs.ptr < rhs.ptr;}
			std::shared_ptr <OperatorDecleration> opp;
			tkitt ptr;
		};
		
		const bool isLtr () const {return ltr;}
		const bool operator < (const OperatorDecleration & rhs) const {if (precidance == rhs.precidance) return pattern < rhs.pattern; return precidance < rhs.precidance;}
		const std::vector <string> & getSymbols () const {return symbols;}
		const string & getPattern () const {return pattern;}
		const TYPE getType () const {return type;}
		
		static TYPE calculateOperatorType (const string & pattern);
		
		OperatorDecleration ();
		OperatorDecleration (int precidance, bool ltr, const string & pattern);
		
	private:
		int precidance;
		bool ltr;
		std::vector <string> symbols;
		string pattern; // space for identifier eg: " * ", " [ ]", " ? : "
		TYPE type;
};


}
}


#endif /*OPERATORDECLERATION_HPP*/