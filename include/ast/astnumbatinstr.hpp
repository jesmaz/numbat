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
		virtual bool isValid () const;
		virtual size_t getBitSize () const {return 0;}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const;
		
		ASTnumbatInstr () {}
		ASTnumbatInstr (const string & instr, const std::vector <ASTnode> & args) : instr (instr), args (args)/*, args (args)*/ {}
	private:
		string instr;
		std::vector <ASTnode> args;
};


}
}


#endif /*ASTNUMBATINSTR_HPP*/