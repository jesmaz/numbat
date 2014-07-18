#ifndef ASTCALLINDEX_HPP
#define ASTCALLINDEX_HPP

#include "astcallable.hpp"
#include "numbattype.hpp"


namespace numbat {
namespace parser {


class ASTcallindex : public ASTbase {
	VISITABLE
	public:
		const shared_ptr <ASTcallable> & getCall () const {return call;}
		const size_t getIndex () const {return index;}
		virtual const ASTnode getASTType () const {return ref->getASTType ();}
		virtual bool isAlias () const {return ref->isAlias ();}
		virtual bool isConst () const {return ref->isConst ();}
		virtual bool isValid () const {return call->isValid ();}
		virtual shared_ptr <NumbatType> getType () const {return ref->getType ();}
		virtual size_t calculateWeight () const {return call->calculateWeight ();}
		virtual size_t getBitSize () const {return ref->getBitSize ();}
		virtual string getIden () const {return ref->getIden ();}
		virtual string toString (const string & indent = "") const {return !index ? call->toString (indent) : ref->toString (indent);}
		
		ASTcallindex () {}
		ASTcallindex (const shared_ptr <ASTcallable> & call, const size_t index) : ref (call->getFunction ()->getType () [index]), call (call), index (index) {}
	private:
		ASTnode ref;
		shared_ptr <ASTcallable> call;
		size_t index;
};


}
}


#endif /*ASTCALLINDEX_HPP*/