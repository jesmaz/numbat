#include <nir/scope.hpp>
#include <parse/tree/ifelse.hpp>
#include <utility/report.hpp>


const nir::Instruction * ParseTreeIfElse::build (nir::Scope * scope) {
	
	//TODO: When unions are avaliable the if-else should use them when the return types are different
	const nir::Instruction * condition = cond->build (scope);
	const nir::Instruction * bodyRet=nullptr, * altRet=nullptr, * alloc=nullptr;
	nir::Block * current=scope->getCurrentBlock ();
	nir::symbol bodyBlock=scope->createBlock (), altBlock=nullptr;
	
	if (alternate) {
		altBlock=scope->createBlock ();
	}
	
	nir::symbol contBlock=scope->createBlock ();
	
	// Build main body of if statement
	scope->changeActiveBlock (bodyBlock);
	bodyRet = body->build (scope);
	
	if (altBlock) {
		// Build main body of else statement
		scope->changeActiveBlock (altBlock);
		altRet = alternate->build (scope);
	}
	
	// Go back to before the if statment to allocate and branch
	scope->changeActiveBlock (current);
	
	if (bodyRet and not bodyRet->getTypes ().empty ()) alloc = scope->allocateVariable (bodyRet->getType ());
	const nir::Instruction * condJump=nullptr;
	if (condition) {
		if (condition->getIdens ().empty ()) {
			report::logMessage (report::ERROR, scope->getSourceFile (), cond->getPos (), "'" + cond->toString (text::PLAIN) + "' is not a ");
			return nullptr;
		}
		condJump = scope->createJump ({condition, condition->getIden ()}, bodyBlock);
	}
	
	if (altBlock) {
		scope->createJump (altBlock);
		// Go back to alt block to store the result and branch
		scope->changeActiveBlock (altBlock);
		if (alloc and altRet and not altRet->getTypes ().empty ()) scope->createPut ({altRet, altRet->getIden ()}, {alloc, alloc->getIden ()});
	} else {
		scope->createJump (contBlock);
	}
	
	// Go back to body block to store the result and branch
	scope->changeActiveBlock (bodyBlock);
	if (alloc) scope->createPut ({bodyRet, bodyRet->getIden ()}, {alloc, alloc->getIden ()});
	scope->createJump (contBlock);
	scope->changeActiveBlock (contBlock);
	if (alloc) return scope->createGet ({alloc, alloc->getIden ()}).instr;
	return nullptr;
	
}

string ParseTreeIfElse::strDump (text::PrintMode mode) {
	
	string s = "if (" + cond->toString (mode) + ") " + body->toString (mode);
	if (alternate) {
		s += " else " + alternate->toString (mode);
	}
	return s;
	
}
