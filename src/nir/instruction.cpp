#include "../../include/nir/instruction.hpp"


namespace nir {


std::vector <const Type *> argumentToType (const std::vector <Argument> & args) {
	std::vector <const Type *> type;
	type.reserve (args.size ());
	for (auto arg : args) {
		type.push_back (arg.instr->getType ());
	}
	return type;
}

std::vector <symbol> argumentToSymbol (const std::vector< Argument > & args) {
	std::vector <symbol> syms;
	syms.reserve (args.size ());
	for (auto arg : args) {
		if (arg.sym) {
			syms.push_back (arg.sym);
		} else {
			syms.push_back (arg.instr->getIden ());
		}
	}
	return syms;
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