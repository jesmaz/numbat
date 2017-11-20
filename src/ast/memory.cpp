#include <ast/memory.hpp>


namespace AST {


string RawInit::toString (text::PrintMode mode) const {
	if (init) {
		return var->toString (mode) + ": " + init->toString (mode);
	} else {
		return var->toString (mode) + ": {}";
	}
}


}
