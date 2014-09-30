#ifndef ASTCALLINDEX_HPP
#define ASTCALLINDEX_HPP

#include "../callable/astcallable.hpp"
#include "../type/numbattype.hpp"


namespace numbat {
namespace parser {


class ASTcallindex : public ASTbase {
	VISITABLE
	public:
		ASTcallable * getCall () {return callp ? callp : call.get ();}
		const ASTcallable * getCall () const {return callp ? callp : call.get ();}
		const size_t getIndex () const {return index;}
		virtual const ASTnode getASTType () const {return ref->getASTType ();}
		virtual bool isAlias () const {return ref->isAlias ();}
		virtual bool isConst () const {return ref->isConst ();}
		virtual bool isValid () const {return (callp ? callp : call.get ())->isValid ();}
		virtual const NumbatType * getType () const {return ref->getType ();}
		virtual size_t calculateWeight () const {return (callp ? callp : call.get ())->calculateWeight ();}
		virtual size_t getBitSize () const {return ref->getBitSize ();}
		virtual string getIden () const {return ref->getIden ();}
		virtual string toString (const string & indent = "") const {return (callp ? callp : call.get ())->toString (indent) + " " + ref->toString ();}
		
		ASTcallindex () {}
		ASTcallindex (const shared_ptr <ASTcallable> & call, const size_t index) : ref (call->getFunction ()->getType () [index]), call (call), index (index) {}
		ASTcallindex (ASTcallable * call, const size_t index) : ref (call->getFunction ()->getType () [index]), callp (call), index (index) {}
	private:
		ASTnode ref;
		shared_ptr <ASTcallable> call;
		ASTcallable * callp;
		size_t index;
};


}
}


#endif /*ASTCALLINDEX_HPP*/