#include <ast/type.hpp>
#include <ast/typecast.hpp>


namespace AST {


string CastToNumber::toString (text::PrintMode mode) const {
	return "(" + getType ()->toString (mode) + " " + node->toString (mode) + ")";
}


}
