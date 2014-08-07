#ifndef ASTTUPLECALL_HPP
#define ASTTUPLECALL_HPP

#include "callable/astcallable.hpp"

#include <list>


namespace numbat {
namespace parser {


class ASTtuplecall : public ASTbase {
	VISITABLE
	public:
		const std::list <ASTnode> & getLhsArgs () const {return lhsArgs;}
		const std::list <ASTnode> & getRhsArgs () const {return rhsArgs;}
		const std::vector <shared_ptr <ASTcallable>> & getCalls () const {return calls;}
		virtual bool isCallable () const {return true;}
		virtual bool isValid () const;
		virtual size_t calculateWeight () const;
		virtual size_t getBitSize () const {return 0;}
		virtual string getIden () const {return iden;}
		virtual string toString (const string & indent = "") const;
		
		ASTtuplecall () {}
		ASTtuplecall (const string & iden, const std::vector <shared_ptr <ASTcallable>> & calls, const std::list <ASTnode> & lhsArgs, const std::list <ASTnode> & rhsArgs) : iden (iden), calls (calls), lhsArgs (lhsArgs), rhsArgs (rhsArgs) {}
	private:
		string iden;
		std::vector <shared_ptr <ASTcallable>> calls;
		std::list <ASTnode> lhsArgs, rhsArgs;
};


}
}


#endif /*ASTTUPLECALL_HPP*/