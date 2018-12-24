#include <ast/flowcontrol.hpp>
#include <ast/function.hpp>
#include <ast/type.hpp>
#include <ast/variable.hpp>
#include <file.hpp>


namespace AST {


bool Array::operator == (const ArrayInit & other) const {return *base == *other.getBaseType ();}

string Array::toString (text::PrintMode mode) const {
	return base->toString (mode) + " []";
}

string ArrayInit::toString (text::PrintMode mode) const {
	if (val) {
		return base->toString (mode) + " [" + length->toString (mode) + "] (" + val->toString (mode) + ")";
	} else {
		return base->toString (mode) + " [" + length->toString (mode) + "]";
	}
}

string Const::toString (text::PrintMode mode) const {
	return "const " + type->toString (mode);
}

string Import::toString (text::PrintMode mode) const {
	return "import " + import->getFileName ();
}

string Inferred::toString (text::PrintMode mode) const {
	return "inferred";
}

string Numeric::toString (text::PrintMode mode) const {
	return std::string () + char (arith) + "int" + std::to_string (minPrecision);
}

string Ptr::toString (text::PrintMode mode) const {
	return type->toString (mode) + "*";
}

string Ref::toString (text::PrintMode mode) const {
	return type->toString (mode) + "&";
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


std::map <BasicArray <TypePtr>, TypePtr> tuples;
std::map <TypePtr, TypePtr> arrayMap, constMap, refMap, ptrMap;
std::map <uint64_t, TypePtr> numericMap;

TypePtr Array::get (const TypePtr & base) {
	auto itt = arrayMap.find (base);
	if (itt != arrayMap.end ()) {
		return itt->second;
	}
	return arrayMap [base] = TypePtr (new Array (base->getPos (), base->getFile (), base));
}

TypePtr Const::get (const TypePtr & base) {
	auto itt = constMap.find (base);
	if (itt != constMap.end ()) {
		return itt->second;
	}
	return constMap [base] = TypePtr (new Const (base->getPos (), base->getFile (), base));
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
		return numericMap [converter.key] = TypePtr (new Numeric ({0, 0}, numbat::File::builtIn (), arith, minPrecision));
	}
}

TypePtr Ptr::get (const TypePtr & base) {
	auto itt = ptrMap.find (base);
	if (itt != ptrMap.end ()) {
		return itt->second;
	}
	return ptrMap [base] = TypePtr (new Ptr (base->getPos (), base->getFile (), base));
}

TypePtr Ref::get (const TypePtr & base) {
	auto itt = refMap.find (base);
	if (itt != refMap.end ()) {
		return itt->second;
	}
	return refMap [base] = TypePtr (new Ref (base->getPos (), base->getFile (), base));
}

TypePtr Struct::tuple (const BasicArray <TypePtr> & vals) {
	auto itt = tuples.find (vals);
	if (itt != tuples.end ()) {
		return itt->second;
	}
	
	string iden = "tuple_";
	for (auto & t : vals) {
		iden += t->toString (text::PrintMode::PLAIN);
	}
	
	auto s = std::make_shared <Struct> (numbat::lexer::position {0, 0}, numbat::File::builtIn (), iden);
	s->members = vals.map <NodePtr> ([](auto&n){return std::make_shared <Variable> (numbat::lexer::position {0, 0}, numbat::File::builtIn (), n, 0, Variable::LOCATION::LOCAL, "");});
	for (size_t i=0, l=vals.size (); i<l; ++i) {
		s->positionMap ["_" + std::to_string (i)] = i;
	}
	return s;
}

}
