#ifndef ASTMEMCPY_HPP
#define ASTMEMCPY_HPP

#include "../callable/astcallable.hpp"
#include "../type/numbattype.hpp"


namespace numbat {
namespace parser {


class ASTmemcpy : public ASTbase {
	VISITABLE
	public:
		const ASTnode & getDest () const {return dest;}
		const ASTnode & getSource () const {return source;}
		const shared_ptr <ASTcallable> & getConv () const {return conv;}
		virtual const ASTnode getASTType () const {return dest->getASTType ();}
		virtual bool isAlias () const {return dest->isAlias ();}
		virtual bool isArray () const {return dest->isArray ();}
		virtual bool isConst () const {return dest->isConst ();}
		virtual bool isParsed () const {return source->isParsed () and dest->isParsed () and (!conv or conv->isParsed ());}
		virtual bool isValid () const {return source->isValid () and dest->isValid () and (!conv or conv->isValid ());}
		virtual const NumbatType * getType () const {return dest->getType ();}
		virtual size_t calculateWeight () const {return dest->calculateWeight () + source->calculateWeight () + (conv ? conv->calculateWeight () : 0);}
		virtual size_t getBitSize () const {return dest->getBitSize ();}
		virtual string getIden () const {return dest->getIden ();}
		virtual string toString (const string & indent = "") const {return indent + dest->toString () + " = " + source->toString ();}
		
		ASTmemcpy (size_t lineNo) : ASTbase (lineNo) {}
		ASTmemcpy (size_t lineNo, const ASTnode & dest, const ASTnode & source, const shared_ptr <ASTcallable> & conv = nullptr) : ASTbase (lineNo), dest (dest),  source (source), conv (conv) {}
	private:
		ASTnode dest, source;
		shared_ptr <ASTcallable> conv;
};


}
}


#endif /*ASTMEMCPY_HPP*/