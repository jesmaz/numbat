#include "../../../include/nir/inst/sub.hpp"


namespace nir {


bool Sub::validate () const {
	
	/*assert (lhs);
	assert (rhs);
	auto * lhsType = lhs->getType ();
	auto * rhsType = rhs->getType ();
	assert (lhsType);
	assert (rhsType);
	assert (lhsType->isNumber ());
	assert (rhsType->isNumber ());
	if (lhsType != rhsType) {
		assert (width, "Bit width must be specified if lhs and rhs are not the same size");
		if (lhsType->numberType () != lhsType->numberType ()) {
			assert (type, "Number type must be defined if lhs and rhs are not the same type");
		}
	}*/
	return true;
	
}

string Sub::strDump (text::PrintMode mode) const {
	
	return "sub." + getType ()->toString (mode) + " " + lhs.toString (mode) + " " + rhs.toString (mode);
	
}


};