#include <nir/inst/jump.hpp>


namespace nir {


string Jump::strDump (text::PrintMode mode) const {
	
	if (condition.instr) {
		return "jump " + condition.toString (mode) + " " + *block->getName ();
	} else {
		return "jump " + *block->getName ();
	}
	
}


};
