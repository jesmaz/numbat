#include <ast/variable.hpp>


namespace AST {


string Variable::toString (text::PrintMode mode) const {
	return "var (" + getType ()->toString (mode) + ") " + identifier;
}


}
