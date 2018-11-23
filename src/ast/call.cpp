#include <ast/call.hpp>
#include <ast/function.hpp>
#include <ast/variable.hpp>

namespace AST {


string Call_n::toString (text::PrintMode mode) const {
	return "<unimplemented>";
}

string Call_0::toString (text::PrintMode mode) const {
	return func->getIden () + " ()";
}

string Call_1::toString (text::PrintMode mode) const {
	return func->getIden () + " (" + arg->toString (mode) + ")";
}

string Call_2::toString (text::PrintMode mode) const {
	return func->getIden () + " (" + lhs->toString (mode) + ", " + rhs->toString (mode) + ")";
}

string SystemCall::toString (text::PrintMode mode) const {
	
	string s = "[system] " + sysName + " (";
	if (not args.empty ()) {
		s += args.front ()->toString (mode);
		for (size_t i=1; i<args.size (); ++i) {
			s += ", " + args [i]->toString (mode);
		}
	}
	return s + ")";
	
}

string Unresolved_Call::toString (text::PrintMode mode) const {
	
	string s = "[unresolved] " + callee->toString (mode) + " (";
	if (not args.empty ()) {
		s += args.front ()->toString (mode);
		for (size_t i=1; i<args.size (); ++i) {
			s += ", " + args [i]->toString (mode);
		}
	}
	return s + ")";
	
}

string Unresolved_Constructor::toString (text::PrintMode mode) const {
	
	string s = var->toString (mode) + " (";
	if (not args.empty ()) {
		s += args.front ()->toString (mode);
		for (size_t i=1; i<args.size (); ++i) {
			s += ", " + args [i]->toString (mode);
		}
	}
	return s + ")";
	
}

string Unresolved_Get_Member::toString (text::PrintMode mode) const {
	
	return "([unresolved] . " + parent->toString (mode) + " " + memberName + ")";
	
}


}
