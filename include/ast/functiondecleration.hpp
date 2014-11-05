#ifndef FUNCTIONDECLERATION_HPP
#define FUNCTIONDECLERATION_HPP

#include "astbase.hpp"
#include "../context.hpp"

#include <set>


namespace numbat {
namespace parser {


struct FunctionDecleration {
	public:
		const ASTnode & getBody () const {return body;}
		const bool hasTag (const string & tag) const {return metaTags.count (tag);}
		const ParsingContext * getContext () const {return context;}
		const std::vector <ASTnode> & getArgs () const {return parameters;}
		const std::vector <ASTnode> & getType () const {return type;}
		const string getIden () const {return iden;}
		size_t calculateWeight () {return weight ? weight : weight = 4, weight = (body ? body->calculateWeight () : 0) + 4;}
		string toString (const string & indent = "");
		void assignBody (ASTnode b) {body = b;}
		
		const bool operator == (const FunctionDecleration & func) {return false;}
		
		FunctionDecleration () {}
		FunctionDecleration (const string & iden, const std::vector <ASTnode> & args, const std::vector <ASTnode> & type, const std::set <string> metaTags, const ParsingContext * context) : iden (iden), metaTags (metaTags), parameters (args), type (type), context (context) {}
	private:
		string iden;
		std::set <string> metaTags;
		std::vector <ASTnode> parameters, type;
		ASTnode body;
		size_t weight=0;
		const ParsingContext * context;
};


}
}

#endif /*FUNCTIONDECLERATION_HPP*/