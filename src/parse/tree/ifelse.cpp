#include <nir/scope.hpp>
#include <parse/tree/ifelse.hpp>


const nir::Instruction * ParseTreeIfElse::build (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	
	const nir::Instruction * condition = cond->build (scope, mode);
	nir::symbol bodyBlock=scope->createBlock (), altBlock;
	
	if (alternate) {
		altBlock=scope->createBlock ();
	}
	
	nir::symbol contBlock=scope->createBlock ();
	const nir::Instruction * condJump = scope->createJump (condition, bodyBlock);
	scope->changeActiveBlock (bodyBlock);
	body->build (scope, mode);
	
	if (alternate) {
		scope->createJump (altBlock);
		scope->changeActiveBlock (altBlock);
		alternate->build (scope, mode);
	}
	
	scope->createJump (contBlock);
	scope->changeActiveBlock (contBlock);
	return condJump;
	
}

string ParseTreeIfElse::strDump (text::PrintMode mode) {
	
	string s = "if (" + cond->toString (mode) + ") " + body->toString (mode);
	if (alternate) {
		s += " else " + alternate->toString (mode);
	}
	return s;
	
}