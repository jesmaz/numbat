#include <nir/parameter.hpp>
#include <nir/type/struct.hpp>

#include <cassert>
#include <set>


const nir::Parameter * nir::Struct::getParam (const string & str) const {
	auto itt = members.find (str);
	if (itt != members.end ()) {
		return itt->second;
	}
	return nullptr;
}

size_t nir::Struct::calculateSize (size_t ptrSize) const {
	size_t s=0;
	for (const Parameter * p : memberArr) {
		s += p->getType ()->calculateSize (ptrSize);
	}
	return s;
}

ssize_t nir::Struct::calculateOffset (size_t ptrSize, const string& iden) const {
	size_t s=0;
	for (const Parameter * p : memberArr) {
		if (*p->getIden () == iden) {
			return s;
		}
		s += p->getType ()->calculateSize (ptrSize);
	}
	return -1;
}

ssize_t nir::Struct::calculateIndex (const nir::Parameter * param) const {
	size_t s=0;
	for (const Parameter * p : memberArr) {
		if (p == param) return s;
		++s;
	}
	return -1;
}

std::string nir::Struct::strDump (text::PrintMode mode) const {
	if (members.empty()) return "{}";
	string s = "{";
	for (auto & m : members) {
		s += m.first + ": " + m.second->toString (mode) + ", ";
	}
	s.pop_back ();
	s.back () = '}';
	return s;
}

void nir::Struct::populate (const BasicArray <const nir::Parameter *> & mem) {
	
	memberArr = mem;
	
	for (const Parameter * p : memberArr) {
		members [*p->getIden ()] = p;
	}
	
}
