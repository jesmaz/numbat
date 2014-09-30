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
		
		ASTfunctionPointer () {}
		ASTfunctionPointer (shared_ptr <FunctionDecleration> function) : function (function.get ()) {}
		ASTfunctionPointer (FunctionDecleration * function) : function (function) {}
		
	private:
		FunctionDecleration * function;
};


}
}


#endif /*ASTFUNCTIONPOINTER_HPP*/