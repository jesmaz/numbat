#ifndef ASTSTRUCTINDEX_HPP
#define ASTSTRUCTINDEX_HPP

#include "astbase.hpp"
#include "numbattype.hpp"


namespace numbat {
namespace parser {


class ASTstructIndex : public ASTbase {
	VISITABLE
	public:
		const ASTnode & getExpr () const {return expr;}
		const size_t & getIndex () const {return index;}
		virtual bool isAlias () const {return expr->isAlias ();}
		virtual bool isConst () const {return expr->isConst ();}
		virtual bool isValid () const {return expr->isValid ();}
		virtual shared_ptr <NumbatType> getType () const {return expr->getType ()->getMembers ()[index]->getType ();}
		virtual size_t getSize () const {return expr->getType ()->getMembers ()[index]->getSize ();}
		virtual string getIden () const {return expr->getType ()->getMembers ()[index]->getIden ();}
		virtual string toString (const string & indent = "") const {return expr->toString (indent) + "." + expr->getType ()->getMembers ()[index]->getIden ();}
		
		ASTstructIndex () {}
		ASTstructIndex (size_t index, const ASTnode & expr) : index (index), expr (expr) {}
	private:
		size_t index;
		ASTnode expr;
};


}
}


#endif /*ASTSTRUCTINDEX_HPP*/