#include <nir/instruction.hpp>
#include <nir/parameter.hpp>
#include <nir/scope.hpp>
#include <parse/tree/variable.hpp>
#include <utility/report.hpp>


namespace parser {


const nir::Instruction * ParseTreeVariable::build (nir::Scope * scope) {
	
	const nir::Instruction * init = nullptr, * var = nullptr;
	const nir::Type * type = nullptr;
	if (inst) {
		init = inst->build (scope);
	}
	
	if (vType->getType () == ParseTreeNode::NodeType::KEYWORD) {
		if (not init) {
			if (not inst) {
				report::logMessage (report::ERROR, scope->getSourceFile (), getPos (), "Unable to determine the type of " + iden->getIden ());
			}
			return nullptr;
		} else if (init->getResults ().empty ()) {
			report::logMessage (report::ERROR, scope->getSourceFile (), inst->getPos (), "'" + inst->toString (text::PLAIN) + "' has no type ");
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
		if (init->getResults ().empty ()) {
			report::logMessage (report::ERROR, scope->getSourceFile (), inst->getPos (), "'" + inst->toString (text::PLAIN) + "' has no type ");
			return nullptr;
		}
		auto val = init;
		if (init->getType () != type and type) {
			val = scope->staticCast (init->getType (), type);
		}
		if (not val or not var) return nullptr;
		return scope->createPut ({val, val->getIden ()}, {var, var->getIden ()});
	}
	return var;
	
}

const nir::Parameter * ParseTreeVariable::buildParameter (nir::Scope * scope) {
	const nir::Instruction * init = nullptr, * var = nullptr;
	const nir::Type * type = nullptr;
	if (inst) {
		init = inst->build (scope);
	}
	if (vType->getType () == ParseTreeNode::NodeType::KEYWORD) {
		if (not init) {
			if (not inst) {
				report::logMessage (report::ERROR, scope->getSourceFile (), getPos (), "Unable to determine the type of " + iden->getIden ());
			}
			return nullptr;
		} else if (init->getResults ().empty ()) {
			report::logMessage (report::ERROR, scope->getSourceFile (), inst->getPos (), "'" + inst->toString (text::PLAIN) + "' has no type ");
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
	if (inst) {
		return new nir::Parameter (inst, type, iden->getIden ());
	} else {
		return new nir::Parameter (type, iden->getIden ());
	}
}

string ParseTreeVariable::strDump (text::PrintMode mode) {
	string s = "(" + vType->toString (mode) + " " + iden->toString (mode) + ")";
	if (inst) {
		s = "(" + s + " : " + inst->toString (mode) + ")";
	}
	return s;
}


}
