#include <nir/parameter.hpp>
#include <nir/type/array.hpp>
#include <nir/type/pointer.hpp>

namespace nir {


std::map <const Type *, Struct *> Array::arrayTypes;


Struct * Array::arrayOf (const Type * t) {
	
	auto itt = arrayTypes.find (t);
	if (itt != arrayTypes.end ()) {
		return itt->second;
	} else {
		Struct * s = new Struct ();
		Parameter * ptr = new Parameter (t->getPointerTo (), "data");
		Parameter * len = new Parameter (t->getPointerTo (), "len");
		s->populate ({ptr, len});
		return arrayTypes [t] = s;
	}
	
}



}