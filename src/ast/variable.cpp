#include <ast/literal.hpp>
#include <ast/variable.hpp>


namespace AST {


string Variable::toString (text::PrintMode mode) const {
	if (currentValue) {
		return "var (" + getType ()->toString (mode) + ") " + identifier + " (" + currentValue->toString (mode) + ")";
	} else {
		return "var (" + getType ()->toString (mode) + ") " + identifier;
	}
}

Variable::Variable (numbat::lexer::position pos, const string & iden, const TypePtr & type) : Node (pos, type), identifier (iden), currentValue (std::make_shared <Number> (pos, "0")) {}

Variable::Variable (numbat::lexer::position pos, const string & iden, const TypePtr & type, const ValPtr & val) : Node (pos, type), identifier (iden), currentValue (val) {}


}
