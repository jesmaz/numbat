#ifndef ASTCALL_HPP
#define ASTCALL_HPP

#include "astcallable.hpp"


namespace numbat {
namespace parser {


class ASTcall : public ASTcallable {
	VISITABLE
	public:
		const shared_ptr <ASTcallable> & getCallee () const {return callee;}
		const std::vector <ASTnode> & getArgs () const {return args;}
		virtual bool isValid () const;
		virtual const FunctionDecleration * getFunction () const {return callee->getFunction ();}
		virtual size_t calculateWeight () const;
		virtual size_t getBitSize () const {return 0;}
		virtual string getIden () const {return callee->getIden ();}
		virtual string toString (const string & indent = "") const;
		
		ASTcall () {}
		ASTcall (const shared_ptr <ASTcallable> & callee, const std::vector <ASTnode> & args) : callee (callee), args (args) {}
	private:
		shared_ptr <ASTcallable> callee;
		std::vector <ASTnode> args;
};


}
}


#endif /*ASTCALL_HPP*/