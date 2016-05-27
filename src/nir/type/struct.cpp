#include <nir/type/struct.hpp>

#include <cassert>
#include <set>



size_t nir::Struct::calculateSize (size_t ptrSize) const {
	size_t s=0;
	for (Type * t : memberArr) {
		s += t->calculateSize (ptrSize);
	}
	return s;
}

std::string nir::Struct::strDump (text::PrintMode mode) const {
	string s = "{";
	for (auto & m : members) {
		s += m.first + ": " + m.second->toString (mode) + ", ";
	}
	s.pop_back ();
	s.back () = '}';
}

nir::Struct::Struct (const std::map <std::string, nir::Type *> & members, const std::vector <nir::Type *> & memberArr) : members (members), memberArr (memberArr) {
	// members and memberArr must be consistent
	std::set <Type *> set (memberArr.begin (), memberArr.end ());
	for (auto & m : members) {
		assert (set.count (m.second));
	}
}