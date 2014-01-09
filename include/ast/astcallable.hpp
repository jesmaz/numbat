#ifndef ASTCALLABLE_HPP
#define ASTCALLABLE_HPP

#include "astbase.hpp"
#include "functiondecleration.hpp"


namespace numbat {
namespace parser {


class ASTcallable : public ASTbase {
	public:
		virtual bool isCallable () const {return true;}
		virtual const shared_ptr <FunctionDecleration> & getFunction () const=0;
};


}
}


#endif /*ASTCALLABLE_HPP*/