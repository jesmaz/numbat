#include <ast/literal.hpp>
#include <ast/variable.hpp>


namespace AST {


string StaticIndex::toString (text::PrintMode mode) const {
	return parent->toString (mode) + "." + std::to_string (index);
}


string Variable::toString (text::PrintMode mode) const {
	if (not currentValue.isNil ()) {
		return "var (" + getType ()->toString (mode) + ") " + identifier + " (" + currentValue.toString (mode) + ")";
	} else {
		return "var (" + getType ()->toString (mode) + ") " + identifier;
	}
}

Variable::Variable (numbat::lexer::position pos, const numbat::File * file, const string & iden, const TypePtr & type) : Node (pos, file, type), identifier (iden) {}

Variable::Variable (numbat::lexer::position pos, const numbat::File * file, const string & iden, const TypePtr & type, const Literal & val) : Node (pos, file, type), identifier (iden), currentValue (val) {}


}
