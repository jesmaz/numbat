#include "../../include/codegen/target.hpp"

#include <cassert>

namespace codegen {

std::map <string, Target::FFunc> * Target::targets=nullptr;

Target * Target::find (const string & iden) {
	if (not targets) return nullptr;
	if (not targets->count (iden)) return nullptr;
	auto * f = (*targets) [iden];
	assert (f);
	auto * t = f();
	assert (t);
	return t;
}

Target::RegTarget::RegTarget (const string & iden, Target::FFunc ffunc) {
	if (not targets) {
		targets = new std::map <string, Target::FFunc>;
	}
	(*targets) [iden] = ffunc;
}


};