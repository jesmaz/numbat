#include <nir/parameter.hpp>
#include <nir/type/array.hpp>
#include <nir/type/number.hpp>
#include <nir/type/pointer.hpp>

namespace nir {


std::map <const Type *, std::unique_ptr <Array>> Array::arrayTypes;


Array * Array::arrayOf (const Type * t) {
	
	auto itt = arrayTypes.find (t);
	if (itt != arrayTypes.end ()) {
		return itt->second.get ();
	} else {
		if (not t) return nullptr;
		Array * a = new Array ();
		Parameter * ptr = new Parameter (t->getPointerTo (), symbol_t ("data"));
		Parameter * len = new Parameter (Number::getNumberType (Type::UINT, 64), symbol_t ("len"));
		a->type.populate ({ptr, len});
		return (arrayTypes [t] = std::unique_ptr <Array> (a)).get ();
	}
	
}



}
