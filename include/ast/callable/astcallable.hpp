#ifndef ASTCALLABLE_HPP
#define ASTCALLABLE_HPP

#include "../astbase.hpp"
#include "../functiondecleration.hpp"


namespace numbat {
namespace parser {


class ASTcallable : public ASTbase {
	public:
		virtual bool isCallable () const {return true;}
		virtual const FunctionDecleration * getFunction () const=0;
		ASTcallable (size_t lineNo) : ASTbase (lineNo) {}
};


}
}


#endif /*ASTCALLABLE_HPP*/