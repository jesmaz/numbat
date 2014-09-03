#ifndef ASTGEP_HPP
#define ASTGEP_HPP

#include "../callable/astcallable.hpp"
#include "../type/numbatpointertype.hpp"


namespace numbat {
namespace parser {


class ASTgep : public ASTbase {
	VISITABLE
	public:
		const ASTnode & getIndex () const {return index;}
		const ASTnode & getRef () const {return ref;}
		virtual const ASTnode getASTType () const {if (const NumbatPointerType * type = dynamic_cast <const NumbatPointerType *> (ref->getType ())) {return type->getDataType ();} return nullptr;}
		virtual bool isAlias () const {return true;}
		virtual bool isCallable () const {return false;}
		virtual bool isConst () const {return ref->isConst ();}
		virtual bool isValid () const {return ref->isValid () and index->isValid ();}
		virtual const NumbatType * getType () const {if (const NumbatPointerType * type = dynamic_cast <const NumbatPointerType *> (ref->getType ())) {return type->getDataType ()->getType ();} return nullptr;}
		virtual size_t calculateWeight () const {return 0;}
		virtual string getIden () const {return ref->getIden ();}
		virtual string toString (const string & indent = "") const {return ref->toString (indent) + " [" + index->toString () + "]";}
		
		ASTgep () {}
		ASTgep (const ASTnode & ref, const ASTnode & index) : ref (ref), index (index) {}
	private:
		ASTnode ref;
		ASTnode index;
};


}
}


#endif /*ASTGEP_HPP*/