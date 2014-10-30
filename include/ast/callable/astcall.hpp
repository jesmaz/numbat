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
		virtual bool isList () const {return true;}
		virtual bool isParsed () const;
		virtual bool isValid () const;
		virtual const FunctionDecleration * getFunction () const {return callee->getFunction ();}
		virtual size_t calculateWeight () const;
		virtual size_t getBitSize () const {return 0;}
		virtual const std::list <ASTnode> getList () const;
		virtual string getIden () const {return callee->getIden ();}
		virtual string toString (const string & indent = "") const;
		
		ASTcall (size_t lineNo) : ASTcallable (lineNo) {}
		ASTcall (size_t lineNo, const shared_ptr <ASTcallable> & callee, const std::vector <ASTnode> & args) : ASTcallable (lineNo), callee (callee), args (args) {}
	private:
		shared_ptr <ASTcallable> callee;
		std::vector <ASTnode> args;
};


}
}


#endif /*ASTCALL_HPP*/