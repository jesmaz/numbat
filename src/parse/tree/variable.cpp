#include <nir/instruction.hpp>
#include <nir/scope.hpp>
#include <parse/tree/variable.hpp>
#include <utility/report.hpp>


const nir::Instruction * ParseTreeVariable::build (nir::Scope * scope) {
	
	const nir::Instruction * init = nullptr, * var = nullptr;
	const nir::Type * type = nullptr;
	if (inst) {
		init = inst->build (scope);
	}
	
	if (vType->getType () == ParseTreeNode::NodeType::KEYWORD) {
		if (not init) {
			if (not inst) {
				report::logMessage (report::ERROR, "", getLine (), getPos (), "Unable to determine the type of " + iden->getIden ());
			}
			return nullptr;
		}
		
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
	
	if (var and init) {
		auto val = init;
		if (init->getType () != type and type) {
			val = scope->staticCast (init, type);
		}
		if (not val or not var) return nullptr;
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
		if (not init) {
			if (not inst) {
				report::logMessage (report::ERROR, "", getLine (), getPos (), "Unable to determine the type of " + iden->getIden ());
			}
			return nullptr;
		}
		type = init->getType ();
		if (vType->getIden () == "ref") {
			type = type->getPointerTo ();
		}
		//TODO: make type const if needed
	} else {
		type = vType->resolveType (scope);
	}
	
	if (not type) return nullptr;
	
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
