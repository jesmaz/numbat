#include "../include/ast/astnumbatinstr.hpp"

namespace numbat {
namespace parser {


bool ASTnumbatInstr::isValid () const {

	bool b=true;
	for (const ASTnode & n : args) {
		b &= n->isValid ();
	}
	return b;
	
}

string ASTnumbatInstr::toString (const string & indent) const {
	
	string ret = indent + "asm 'numbat' : '" + instr + "' (";
	for (const ASTnode & arg : args) {
		ret += arg->toString () + ", ";
	}
	return ret + ")";
	
}


}
}