#include <cassert>
#include <nir/parameter.hpp>
#include <nir/type/tuple.hpp>
#include <set>


std::set <const BasicArray <const nir::Type *> *, nir::Tuple::CmpPtr> nir::Tuple::tuples;
std::set <const nir::Tuple *> nir::Tuple::tPtrs;


size_t nir::Tuple::calculateSize (size_t ptrSize) const {
	size_t s=0;
	for (const Type * t : *memberArr) {
		s += t->calculateSize (ptrSize);
	}
	return s;
}

ssize_t nir::Tuple::calculateOffset (size_t ptrSize, size_t index) const {
	size_t s=0;
	size_t i=0;
	for (const Type * t : *memberArr) {
		if (i == index) {
			return s;
		}
		s += t->calculateSize (ptrSize);
	}
	return -1;
}

std::string nir::Tuple::strDump (text::PrintMode mode) const {
	if (memberArr->empty()) return "{}";
	string s = "{";
	for (auto & m : *memberArr) {
		s += m->toString (mode) + ", ";
	}
	s.pop_back ();
	s.back () = '}';
	return s;
}

void nir::Tuple::replaceTuple (const BasicArray <const Type *> & args) {
	
	auto itt = tuples.find (&args);
	auto end = tuples.end ();
	
	if (itt == end) {
		tuples.insert (new const BasicArray <const Type *> (args));
	} else {
		memberArr = *itt;
	}
	
}

const nir::Tuple * nir::Tuple::getTuple (const BasicArray <const Type *> & args) {
	
	auto t = newTuple ();
	t->replaceTuple (args);
	return t;
	
}

nir::Tuple * nir::Tuple::newTuple () {
	
	auto t = new nir::Tuple ();
	tPtrs.insert (t);
	return t;
	
}
