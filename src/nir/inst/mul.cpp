#include "../../../include/nir/inst/mul.hpp"


namespace nir {


bool Mul::validate () const {
	
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

string Mul::strDump (text::PrintMode mode) const {
	
	return "mul." + getType ()->toString (mode) + " " + lhs->printIden (mode) + " " + rhs->printIden (mode);
	
}


};
