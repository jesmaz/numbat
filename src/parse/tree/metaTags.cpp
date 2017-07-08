#include <parse/tree/metaTags.hpp>


namespace parser {


const nir::Instruction * MetaTags::build (nir::Scope * scope) {
	
	const auto * cmpt = componant->build (scope);
	for (auto itt = tags.rbegin (), end = tags.rend (); itt!=end; ++itt) {
		//TODO: Implement this method. Will require some attention to defining meta tags in the first place
		// One option is: def @tag (function f) -> (function)
		// presumably this function would be staticly interpeted here
		// This is fine, other than two things. What properties should functions be allowed to have?
		// And what happens when (not if) a user tries to modify the global state of the program in a meta tag?
	}
	return cmpt;
	
}

void MetaTags::declare (nir::Scope * scope) {
	componant->declare (scope);
}

string MetaTags::strDump (text::PrintMode mode) {
	string s = "(";
	for (auto & tag : tags) {
		s += "@" + tag->toString (mode);
		s += " ";
	}
	return s + componant->toString (mode) + ")";
}


}
