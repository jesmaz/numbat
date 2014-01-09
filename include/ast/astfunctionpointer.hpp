#ifndef ASTFUNCTIONPOINTER_HPP
#define ASTFUNCTIONPOINTER_HPP

#include "astcallable.hpp"


namespace numbat {
namespace parser {


class ASTfunctionPointer : public ASTcallable {
	VISITABLE
	public:
		virtual bool isValid () const {return true;}
		virtual const shared_ptr <FunctionDecleration> & getFunction () const {return function;}
		virtual size_t getSize () const {return 0;}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {return indent + "'" + function->getIden () + "'";}
		
		ASTfunctionPointer () {}
		ASTfunctionPointer (shared_ptr <FunctionDecleration> function) : function (function) {}
		
	private:
		shared_ptr <FunctionDecleration> function;
};


}
}


#endif /*ASTFUNCTIONPOINTER_HPP*/