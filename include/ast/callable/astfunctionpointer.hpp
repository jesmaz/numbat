#ifndef ASTFUNCTIONPOINTER_HPP
#define ASTFUNCTIONPOINTER_HPP

#include "astcallable.hpp"


namespace numbat {
namespace parser {


class ASTfunctionPointer : public ASTcallable {
	VISITABLE
	public:
		virtual bool isParsed () const {return true;}
		virtual bool isValid () const {return true;}
		virtual const FunctionDecleration * getFunction () const {return function;}
		virtual size_t calculateWeight () const {return function->calculateWeight ();}
		virtual size_t getBitSize () const {return 0;}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {return indent + "'" + function->getIden () + "'";}
		
		ASTfunctionPointer (size_t lineNo) : ASTcallable (lineNo) {}
		ASTfunctionPointer (size_t lineNo, shared_ptr <FunctionDecleration> function) : ASTcallable (lineNo), function (function.get ()) {}
		ASTfunctionPointer (size_t lineNo, FunctionDecleration * function) : ASTcallable (lineNo), function (function) {}
		
	private:
		FunctionDecleration * function;
};


}
}


#endif /*ASTFUNCTIONPOINTER_HPP*/