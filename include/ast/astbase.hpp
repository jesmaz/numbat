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


class ASTbase : public visitor::BaseVisitable {
	public:
		virtual bool isAlias () const {return false;}
		virtual bool isCallable () const {return false;}
		virtual bool isConst () const {return false;}
		virtual bool isList () const {return false;}
		virtual bool isRaw () const {return false;}
		virtual bool isValid () const=0;
		virtual shared_ptr <NumbatType> getType () const {return nullptr;}
		virtual size_t calculateWeight () const {return 0;};
		virtual size_t getSize () const=0;
		virtual string getIden () const=0;
		virtual string toString (const string & indent = "") const=0;
		ASTbase () {}
};

typedef shared_ptr <ASTbase> ASTnode;

class ASTlistInterface {
	public:
		virtual const std::vector <ASTnode> getElements () const=0;
};


}
}


#endif /*ASTBASE_HPP*/