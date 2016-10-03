#include "../../include/nir/instruction.hpp"


namespace nir {


BasicArray <const Type *> argumentToType (const BasicArray <Argument> & args) {
	return args.map <const Type *> ([](const Argument & a){return a.instr->getType ();});
}

BasicArray <symbol> argumentToSymbol (const BasicArray <Argument> & args) {
	return args.map <symbol> ([](const Argument & arg){
		if (arg.sym) {
			return arg.sym;
		} else {
			return arg.instr->getIden ();
		}
	});
}


string Argument::toString (text::PrintMode mode) const {
	
	if (instr) {
		if (sym) {
			return *sym;
		} else {
			return instr->printIden (mode);
		}
	} else {
		return "nullptr";
	}
	
}


string Instruction::printIden (text::PrintMode mode) const {
	if (this) {
		string s;
		for (auto iden : idens) {
			if (iden) {
				s += *iden;
			} else {
				s += strDump (mode);
			}
			s += " ";
		}
		s.pop_back ();
		return s;
	} else {
		return "nullptr";
	}
}

string Instruction::toString (text::PrintMode mode) const {
	if (this) {
		string s;
		for (auto iden : idens) {
			if (iden) {
				s += *iden;
			}
		}
		return s + ": " + strDump (mode);;
	} else {
		return "nullptr";
	}
}


};
