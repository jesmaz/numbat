#include <nir/parameter.hpp>
#include <nir/type/array.hpp>
#include <nir/type/number.hpp>
#include <nir/type/pointer.hpp>

namespace nir {


const size_t Array::DATA_INDEX=0, Array::LENGTH_INDEX=1;
std::map <const Type *, std::unique_ptr <Array>> Array::arrayTypes;


Array * Array::arrayOf (const Type * t) {
	
	auto itt = arrayTypes.find (t);
	if (itt != arrayTypes.end ()) {
		return itt->second.get ();
	} else {
		if (not t) return nullptr;
		return (arrayTypes [t] = std::unique_ptr <Array> (new Array (t))).get ();
	}
	
}

Array::Array (const Type * elementType) : type (Tuple::getTuple ({elementType->getPointerTo (), Number::getNumberType (Type::UINT, 64)})), elementType (elementType) {}



}
