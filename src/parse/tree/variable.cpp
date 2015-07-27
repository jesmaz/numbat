#include "../../../include/parse/tree/variable.hpp"

numbat::parser::ASTnode ParseTreeVariable::build (numbat::parser::NumbatScope * scope) {
	numbat::parser::ASTnode init = nullptr, type = nullptr;
	if (inst) {
		init = inst->build (scope);
	}
	if (vType->getType () == ParseTreeNode::NodeType::KEYWORD) {
		assert (inst);
		assert (init);
		auto * nbtType = init->getType ();
		assert (nbtType);
		type = numbat::parser::ASTnode (new numbat::parser::ASTtype (getLine (), false, false, nbtType));
	} else {
		type = vType->build (scope);
	}
	assert (type);
	assert (type->getType ());
	numbat::parser::NumbatVariable * var = numbat::parser::createVariable (scope, type, init, iden->getIden (), false, false);
	if (var) {
		return numbat::parser::ASTnode (new numbat::parser::ASTvariable (getLine (), var));
	} else {
		std::cerr << iden->getIden () << std::endl;
		assert (0);
		return nullptr;//generateError (pos, "'" + pos.itt->iden + "' already declared in this scope");
	}
}

string ParseTreeVariable::strDump (text::PrintMode mode) {
	string s = "(" + vType->toString (mode) + " " + iden->toString (mode) + ")";
	if (inst) {
		s = "(" + s + " : " + inst->toString (mode) + ")";
	}
	return s;
}