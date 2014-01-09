#ifndef FUNCTIONDECLERATION_HPP
#define FUNCTIONDECLERATION_HPP

#include "astbase.hpp"


namespace numbat {
namespace parser {


struct FunctionDecleration {
	public:
		const ASTnode & getBody () const {return body;}
		const std::vector <ASTnode> & getArgs () const {return parameters;}
		const std::vector <ASTnode> & getType () const {return type;}
		const string getIden () const {return iden;}
		string toString (const string & indent = "");
		void assignBody (ASTnode b) {body = b;}
		
		FunctionDecleration () {}
		FunctionDecleration (const string & iden, const std::vector <ASTnode> & args, const std::vector <ASTnode> & type) : iden (iden), parameters (args), type (type) {}
	private:
		string iden;
		std::vector <ASTnode> parameters, type;
		ASTnode body;
};


}
}

#endif /*FUNCTIONDECLERATION_HPP*/