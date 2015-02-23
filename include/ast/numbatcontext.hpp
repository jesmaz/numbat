#ifndef NUMBATCONTEXT_HPP
#define NUMBATCONTEXT_HPP

#include "astbase.hpp"


namespace numbat {
namespace parser {


class AbstractSyntaxTree;
class FunctionDecleration;
class NumbatType;
class NumbatVariable;


class NumbatContext {
	
	public:
		
		virtual std::vector <FunctionDecleration *> findFunctions (const string & iden)=0;
		virtual const NumbatType * getType (const string & iden)=0;
		virtual const NumbatVariable * getVariable (const string & iden)=0;
		
		ASTnode resolveSymbol (const string & iden, ASTnode parent=nullptr, bool cascade=true, bool local=true) const;
		
	protected:
	private:
		
		virtual const NumbatContext * const getParent ()const=0;
		NumbatContext * getParent () {return const_cast <NumbatContext *> (static_cast <const NumbatContext *> (this)->getParent ());}
		
};

}
}


#endif /*NUMBATCONTEXT_HPP*/