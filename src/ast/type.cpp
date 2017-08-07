#include <ast/type.hpp>


namespace AST {


string Const::toString (text::PrintMode mode) const {
	return "const " + type->toString (mode);
}

string Ref::toString (text::PrintMode mode) const {
	return type->toString (mode) + "*";
}

string Inferred::toString (text::PrintMode mode) const {
	return "inferred";
}

string Array::toString (text::PrintMode mode) const {
	return base->toString (mode) + " []";
}

string ArrayInit::toString (text::PrintMode mode) const {
	return base->toString (mode) + " [" + length->toString (mode) + "] (" + val->toString (mode) + ")";
}

string Numeric::toString (text::PrintMode mode) const {
	return std::string () + char (arith) + "int" + std::to_string (minPrecision);
}

string Struct::toString (text::PrintMode mode) const {
	string s = getName () + " [struct] {";
	if (not members.empty ()) {
		s += members.front ()->toString (mode);
		for (size_t i=1, l=members.size (); i<l; ++i) {
			s += ", " + members [i]->toString (mode);
		}
	}
	return s + "}";
}


std::map <TypePtr, TypePtr> arrayMap, constMap, refMap;
std::map <uint64_t, TypePtr> numericMap;

TypePtr Array::get (const TypePtr & base) {
	auto itt = arrayMap.find (base);
	if (itt != arrayMap.end ()) {
		return itt->second;
	}
	return arrayMap [base] = TypePtr (new Array (base->getPos (), base));
}

TypePtr Const::get (const TypePtr & base) {
	auto itt = constMap.find (base);
	if (itt != constMap.end ()) {
		return itt->second;
	}
	return constMap [base] = TypePtr (new Const (base->getPos (), base));
}

TypePtr Ref::get (const TypePtr & base) {
	auto itt = refMap.find (base);
	if (itt != refMap.end ()) {
		return itt->second;
	}
	return refMap [base] = TypePtr (new Ref (base->getPos (), base));
}

TypePtr Numeric::get (ArithmaticType arith, uint32_t minPrecision) {
	union {
		struct {
			ArithmaticType arith;
			uint32_t minPrecision;
		} orig;
		uint64_t key;
	} converter;
	converter.key = 0;
	converter.orig.arith = arith;
	converter.orig.minPrecision = minPrecision;
	auto itt = numericMap.find (converter.key);
	if (itt != numericMap.end ()) {
		return itt->second;
	} else {
		return numericMap [converter.key] = TypePtr (new Numeric ({0, 0}, arith, minPrecision));
	}
}

}
