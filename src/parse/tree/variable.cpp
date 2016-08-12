#include <nir/instruction.hpp>
#include <nir/scope.hpp>
#include <parse/tree/variable.hpp>


const nir::Instruction * ParseTreeVariable::build (nir::Scope * scope) {
	
	const nir::Instruction * init = nullptr, * var = nullptr;
	const nir::Type * type = nullptr;
	if (inst) {
		init = inst->build (scope);
	}
	if (vType->getType () == ParseTreeNode::NodeType::KEYWORD) {
		assert (inst);
		assert (init);
		type = init->getType ();
		if (vType->getIden () == "ref") {
			type = type->getPointerTo ();
		}
		//TODO: make type const if needed
		var = scope->allocateVariable (type, iden->getIden ());
	} else {
		var = vType->buildAllocate (scope, iden->getIden ());
		type = vType->resolveType (scope);
	}
	assert (var);
	std::cerr << typeid (*iden).name () << std::endl;
	std::cerr << iden->toString () << std::endl;
	std::cerr << iden->getIden () << std::endl;
	
	
	if (init) {
		auto val = init;
		if (init->getType () != type) {
			val = scope->staticCast (init, type);
		}
		return scope->createPut ({val, val->getIden ()}, {var, var->getIden ()});
	}
	return var;
	
}

const nir::Instruction * ParseTreeVariable::buildParameter (nir::Scope * scope) {
	const nir::Instruction * init = nullptr, * var = nullptr;
	const nir::Type * type = nullptr;
	if (inst) {
		init = inst->build (scope);
	}
	if (vType->getType () == ParseTreeNode::NodeType::KEYWORD) {
		assert (inst);
		assert (init);
		type = init->getType ();
		if (vType->getIden () == "ref") {
			type = type->getPointerTo ();
		}
		//TODO: make type const if needed
	} else {
		type = vType->resolveType (scope);
	}
	assert (type);
	std::cerr << typeid (*iden).name () << std::endl;
	std::cerr << iden->toString () << std::endl;
	std::cerr << iden->getIden () << std::endl;
	
	nir::symbol sym = nullptr;
	if (init) sym = init->getIden ();
	return scope->createParameter (type, {init, sym}, iden->getIden ());
}

string ParseTreeVariable::strDump (text::PrintMode mode) {
	string s = "(" + vType->toString (mode) + " " + iden->toString (mode) + ")";
	if (inst) {
		s = "(" + s + " : " + inst->toString (mode) + ")";
	}
	return s;
}