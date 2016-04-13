#include "../../include/ast/asterror.hpp"
#include "../../include/ast/control/astbody.hpp"
#include "../../include/ast/memory/aststructindex.hpp"
#include "../../include/ast/memory/astvariable.hpp"

namespace numbat {
namespace parser {


ASTnode ASTbase::resolveSymbol (const string & iden, const ASTnode & exp) {
	ASTnode val = nullptr;
	auto type = exp->getType ();
	if (type) {
		auto index = type->findMember (iden);
		if (0 <= index) {
			ASTnode mem = type->getMembers() [index];
			if (mem->isGlobal ()) {
				ASTvariable * var = dynamic_cast <ASTvariable *> (mem.get ());
				if (var) {
					val = var->getVariable ()->getInit ();
				} else {
					val = mem;
				}
			} else {
				val = ASTnode (new ASTstructIndex (0, index, exp));
			}
		}
	}
	if (!val) {
		val = exp->resolveSymbol (iden);
	}
	return val;
}

ASTnode ASTbase::resolveSymbol (const string & iden) const {
	return ASTnode (new ASTerror (0, "'" + iden + "' could not be resolved"));
}

const ASTnode ASTbase::getLength (const ASTnode & exp) {
	
	ASTnode val = nullptr;
	ssize_t index = exp->getLengthIndex ();
	if (0 <= index) {
		val = ASTnode (new ASTstructIndex (0, index, exp));
	}
	return val;
	
}

const ASTnode ASTbase::getArrayType () const {
	
	auto type = getType ();
	ASTnode t = nullptr;
	if (type) {
		t = type->getArrayType ();
	}
	return t;
	
}

ssize_t ASTbase::getLengthIndex () const {
	
	auto type = getType ();
	if (type) {
		return type->getLengthIndex ();
	}
	return -1;
	
}


}
}