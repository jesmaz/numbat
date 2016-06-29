#include "../../include/nir/instruction.hpp"


namespace nir {


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