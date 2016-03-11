#include "../../include/nir/type.hpp"
#include "../../include/nir/type/pointer.hpp"

namespace nir {


const Type * Type::getPointerTo() const {
	
	return PointerType::pointerTo (this);
	
}


};