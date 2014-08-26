#ifndef ASTBASE_HPP
#define ASTBASE_HPP

#include "../visitor.hpp"

#include <memory>
#include <vector>


using std::shared_ptr;
using std::string;
using std::unique_ptr;


namespace numbat {
namespace parser {


class ASTfunctionPointer;
class NumbatType;

class ASTbase;



typedef shared_ptr <ASTbase> ASTnode;


class ASTbase : public visitor::BaseVisitable {
	public:
		static const ASTnode getLength (const ASTnode & exp);
		static ASTnode resolveSymbol (const string & iden, const ASTnode & exp);
		virtual ASTnode resolveSymbol (const string & iden) const;
		virtual const ASTnode getArrayType () const;
		virtual const ASTnode getASTType () const {return nullptr;}
		virtual bool isAlias () const {return false;}
		virtual bool isArray () const {return false;}
		virtual bool isCallable () const {return false;}
		virtual bool isConst () const {return false;}
		virtual bool isList () const {return false;}
		virtual bool isNil () const {return false;}
		virtual bool isRaw () const {return false;}
		virtual bool isValid () const=0;
		virtual shared_ptr <NumbatType> getType () const {return nullptr;}
		virtual size_t calculateWeight () const=0;
		//DEPRECATED
		virtual size_t getBitSize () const {return 0;}
		virtual string getIden () const=0;
		virtual string toString (const string & indent = "") const=0;
		virtual ssize_t getLengthIndex () const;
		ASTbase () {}
};

class ASTwrapper : public ASTbase {
	public:
		virtual ASTnode resolveSymbol (const string & iden) const {return node->resolveSymbol (iden);}
		virtual const ASTnode getArrayType () const {return node->getArrayType ();}
		virtual const ASTnode getASTType () const {return node->getASTType();}
		virtual bool isAlias () const {return node->isAlias ();}
		virtual bool isArray () const {return node->isArray ();}
		virtual bool isCallable () const {return node->isCallable ();}
		virtual bool isConst () const {return node->isConst ();}
		virtual bool isList () const {return node->isList ();}
		virtual bool isNil () const {return node->isNil ();}
		virtual bool isRaw () const {return node->isRaw ();}
		virtual bool isValid () const {return node->isValid ();}
		virtual shared_ptr <NumbatType> getType () const {return node->getType ();}
		virtual size_t calculateWeight () const {return node->calculateWeight ();}
		virtual size_t getBitSize () const {return node->getBitSize ();}
		virtual string getIden () const {return node->getIden ();}
		virtual string toString (const string & indent = "") const {return node->toString (indent);}
		virtual ssize_t getLengthIndex () const {return node->getLengthIndex ();}
		virtual void accept (visitor::BaseVisitor & vis) {const_cast <ASTbase *> (node)->accept (vis);}
		ASTwrapper (const ASTbase * node) : node (node) {}
	private:
		const ASTbase * node;
};

class ASTlistInterface {
	public:
		virtual const std::vector <ASTnode> getElements () const=0;
};


}
}


#endif /*ASTBASE_HPP*/