#include <nir/instruction.hpp>
#include <nir/scope.hpp>
#include <parse/tree/resolvescope.hpp>


namespace parser {


string ResolveScope::strDump (text::PrintMode mode) {
	
	return parent->toString (mode) + "." + iden;
	
}


}
