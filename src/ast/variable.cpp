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

Variable::Variable (numbat::lexer::position pos, const numbat::File * file, const string & iden, const TypePtr & type) : Node (pos, file, type), identifier (iden), currentValue (std::make_shared <Number> (pos, file, "0")) {}

Variable::Variable (numbat::lexer::position pos, const numbat::File * file, const string & iden, const TypePtr & type, const ValPtr & val) : Node (pos, file, type), identifier (iden), currentValue (val) {}


}
