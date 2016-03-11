#include "../../include/nir/instruction.hpp"


namespace nir {


string Instruction::printIden (text::PrintMode mode) const {
	if (this) {
		if (iden) {
			return *iden;
		} else {
			return strDump (mode);
		}
	} else {
		return "nullptr";
	}
}

string Instruction::toString (text::PrintMode mode) const {
	if (this) {
		if (iden) {
			return *iden + ": " + strDump (mode);
		} else {
			return ": " + strDump (mode);
		}
	} else {
		return "nullptr";
	}
}


};