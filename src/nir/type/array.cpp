#include <nir/parameter.hpp>
#include <nir/type/array.hpp>
#include <nir/type/pointer.hpp>

namespace nir {


std::map <const Type *, Array *> Array::arrayTypes;


Array * Array::arrayOf (const Type * t) {
	
	auto itt = arrayTypes.find (t);
	if (itt != arrayTypes.end ()) {
		return itt->second;
	} else {
		Array * a = new Array ();
		Parameter * ptr = new Parameter (t->getPointerTo (), "data");
		Parameter * len = new Parameter (t->getPointerTo (), "len");
		a->type.populate ({ptr, len});
		return arrayTypes [t] = a;
	}
	
}



}
