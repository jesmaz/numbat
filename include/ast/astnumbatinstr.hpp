#ifndef ASTNUMBATINSTR_HPP
#define ASTNUMBATINSTR_HPP

#include "astbase.hpp"


namespace numbat {
namespace parser {


class ASTnumbatInstr : public ASTbase {
	VISITABLE
	public:
		const string & getInstr () const {return instr;}
		const std::vector <ASTnode> getArgs () const {return args;}
		virtual const ASTnode getASTType () const {return type;}
		virtual bool isParsed () const {for (auto & arg : args ) if (!arg->isParsed ()) return false; return true;}
		virtual bool isValid () const;
		virtual const NumbatType * getType () const {return type->getType ();}
		virtual size_t calculateWeight () const;
		virtual size_t getBitSize () const {return type->getBitSize ();}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const;
		
		ASTnumbatInstr () {}
		ASTnumbatInstr (const string & instr, const std::vector <ASTnode> & args, const ASTnode & type) : instr (instr), args (args), type (type) {}
	private:
		string instr;
		std::vector <ASTnode> args;
		ASTnode type;
};


}
}


#endif /*ASTNUMBATINSTR_HPP*/