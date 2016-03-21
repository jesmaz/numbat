#include "../../../include/nir/instruction.hpp"
#include "../../../include/nir/scope.hpp"
#include "../../../include/parse/tree/variable.hpp"


const nir::Instruction * ParseTreeVariable::build (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	
	const nir::Instruction * init = nullptr, * var = nullptr;
	const nir::Type * type = nullptr;
	if (inst) {
		init = inst->build (scope, ParseTreeNode::BuildMode::NORMAL);
	}
	if (vType->getType () == ParseTreeNode::NodeType::KEYWORD) {
		assert (inst);
		assert (init);
		type = init->getType ();
	} else {
		auto * instr = vType->build (scope, ParseTreeNode::BuildMode::PARAMETER);
		assert (instr);
		type = instr->getType ();
	}
	assert (type);
	std::cerr << typeid (*iden).name () << std::endl;
	std::cerr << iden->toString () << std::endl;
	std::cerr << iden->getIden () << std::endl;
	if (mode == ParseTreeNode::BuildMode::PARAMETER) {
		return scope->createParameter (type, init, iden->getIden ());
	} else {
		var = scope->allocateVariable (type, iden->getIden ());
	}
	if (init) {
		auto val = init;
		if (init->getType () != type) {
			val = scope->staticCast (init, type);
		}
		return scope->createPut (val, var);
	}
	return var;
	
}

string ParseTreeVariable::strDump (text::PrintMode mode) {
	string s = "(" + vType->toString (mode) + " " + iden->toString (mode) + ")";
	if (inst) {
		s = "(" + s + " : " + inst->toString (mode) + ")";
	}
	return s;
}