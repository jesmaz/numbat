#include <ast/flowcontrol.hpp>
#include <ast/type.hpp>
#include <ast/variable.hpp>


namespace AST {


string And::toString (text::PrintMode mode) const {
	return "or (" + first->toString (mode) + ", " + second->toString (mode) + ")";
}

string IfElse::toString (text::PrintMode mode) const {
	if (getType ()) {
		if (alt) {
			return "(if " + getType ()->toString (mode) + " (" + cond->toString (mode) + ") " + body->toString (mode) + " else " + alt->toString (mode) + ")";
		} else {
			return "(if " + getType ()->toString (mode) + " (" + cond->toString (mode) + ") " + body->toString (mode) + ")";
		}
	} else {
		if (alt) {
			return "(if (" + cond->toString (mode) + ") " + body->toString (mode) + " else " + alt->toString (mode) + ")";
		} else {
			return "(if (" + cond->toString (mode) + ") " + body->toString (mode) + ")";
		}
	}
}

IfElse::IfElse (numbat::lexer::position pos, const numbat::File * file, const VarPtr & var, const NodePtr & cond, const NodePtr & body, const NodePtr & alt) : Node (pos, file, var->getType ()), cond (cond), body (body), alt (alt), retVar (var) {
	
}

string Loop::toString (text::PrintMode mode) const {
	return "loop (" + init->toString (mode) + ", " + cond->toString (mode) + ", " + step->toString (mode) + ", " + body->toString (mode) + ")";
}

string Or::toString (text::PrintMode mode) const {
	return "or (" + first->toString (mode) + ", " + second->toString (mode) + ")";
}

string Unresolved_IfElse::toString (text::PrintMode mode) const {
	if (getType ()) {
		if (alt) {
			return "(if " + getType ()->toString (mode) + " (" + cond->toString (mode) + ") " + body->toString (mode) + " else " + alt->toString (mode) + ")";
		} else {
			return "(if " + getType ()->toString (mode) + " (" + cond->toString (mode) + ") " + body->toString (mode) + ")";
		}
	} else {
		if (alt) {
			return "(if (" + cond->toString (mode) + ") " + body->toString (mode) + " else " + alt->toString (mode) + ")";
		} else {
			return "(if (" + cond->toString (mode) + ") " + body->toString (mode) + ")";
		}
	}
}

string Unresolved_Loop::toString (text::PrintMode mode) const {
	return "loop (" + init->toString (mode) + ", " + cond->toString (mode) + ", " + step->toString (mode) + ", " + body->toString (mode) + ")";
}


}
