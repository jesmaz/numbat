#ifndef ASTBASE_HPP
#define ASTBASE_HPP

#include "../visitor.hpp"

#include <list>
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
		virtual bool isGlobal () const {return false;}
		virtual bool isList () const {return false;}
		virtual bool isNil () const {return false;}
		virtual bool isParsed () const=0;
		virtual bool isRaw () const {return false;}
		virtual bool isReturned () const {return false;}
		virtual bool isValid () const=0;
		virtual const NumbatType * getType () const {return nullptr;}
		virtual size_t calculateWeight () const=0;
		//DEPRECATED
		virtual size_t getBitSize () const {return 0;}
		virtual const std::list <ASTnode> getList () const {return std::list <ASTnode> ();}
		virtual string getIden () const=0;
		virtual string toString (const string & indent = "") const=0;
		virtual ssize_t getLengthIndex () const;
		size_t getLineNo () {return lineNo;}
		ASTbase (size_t lineNo) : lineNo (lineNo) {}
	protected:
	private:
		size_t lineNo;
};

class ASTlistInterface {
	public:
		virtual const std::vector <ASTnode> getElements () const=0;
};


}
}


#endif /*ASTBASE_HPP*/