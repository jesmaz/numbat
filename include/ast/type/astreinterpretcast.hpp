#ifndef ASTREINTERPRET_HPP
#define ASTREINTERPRET_HPP

#include "../astbase.hpp"


namespace numbat {
namespace parser {


class ASTreinterpretCast : public ASTbase {
	VISITABLE
	public:
		const ASTnode getArg () const {return arg;}
		const bool loadSource () const {return loadSrc;}
		virtual const ASTnode getArrayType () const {return type->getArrayType ();}
		virtual const ASTnode getASTType () const {return type;}
		virtual bool isAlias () const {return type->isAlias ();}
		virtual bool isArray () const {return type->isArray ();}
		virtual bool isCallable () const {return type->isCallable ();}
		virtual bool isConst () const {return type->isConst ();}
		virtual bool isList () const {return type->isList ();}
		virtual bool isNil () const {return type->isNil ();}
		virtual bool isParsed () const {return arg->isParsed () and type->isParsed ();}
		virtual bool isRaw () const {return type->isRaw ();}
		virtual bool isValid () const {return arg->isValid () and type->isValid ();}
		virtual const NumbatType * getType () const {return type->getType ();}
		virtual size_t calculateWeight () const {return arg->calculateWeight ();}
		virtual size_t getBitSize () const {return type->getBitSize ();}
		virtual string getIden () const {return "AST reinterpret";}
		virtual string toString (const string & indent = "") const {return indent + arg->toString () + " as " + type->toString ();}
		
		ASTreinterpretCast (size_t lineNo) : ASTbase (lineNo) {}
		ASTreinterpretCast (size_t lineNo, const ASTnode & arg, const ASTnode & type) : ASTbase (lineNo), arg (arg), type (type), loadSrc (false) {}
		ASTreinterpretCast (size_t lineNo, const ASTnode & arg, const ASTnode & type, bool loadSource) : ASTbase (lineNo), arg (arg), type (type), loadSrc (loadSource) {}
	private:
		ASTnode arg, type;
		bool loadSrc;
};


}
}


#endif /*ASTREINTERPRET_HPP*/