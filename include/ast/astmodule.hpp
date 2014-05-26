#ifndef ASTMODULE_HPP
#define ASTMODULE_HPP

#include "astbase.hpp"
#include "../module.hpp"


namespace numbat {
namespace parser {


class ASTmodule : public ASTbase {
	public:
		virtual bool isAlias () const {return false;}
		virtual bool isConst () const {return false;}
		virtual bool isNil () const {return false;}
		virtual bool isValid () const {return true;}
		virtual size_t getBitSize () const {return 0;}
		virtual string getIden () const {return "module";}
		virtual string toString (const string & indent = "") const {return indent + "module";}
		
		ASTnil () {}
	private:
		shared_ptr <Module> module;
};


}
}


#endif /*ASTMODULE_HPP*/