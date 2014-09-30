#include "../include/ast/callable/astcall.hpp"
#include "../include/ast/memory/astcallindex.hpp"

namespace numbat {
namespace parser {


bool ASTcall::isParsed () const {
	
	bool b=callee->isParsed ();
	for (const ASTnode & n : args) {
		b &= n->isParsed ();
	}
	return b;
	
}

bool ASTcall::isValid () const {
	
	bool b=callee->isValid ();
	for (const ASTnode & n : args) {
		b &= n->isValid ();
	}
	return b;
	
}

size_t ASTcall::calculateWeight () const {
	
	size_t weight = callee->calculateWeight ();
	for (const ASTnode & arg : args) {
		weight += arg->calculateWeight ();
	}
	return weight;
	
}

const std::list <ASTnode> ASTcall::getList () const {
	
	auto & types = callee->getFunction ()->getType ();
	std::list <ASTnode> list;
	size_t l = types.size ();
	for (size_t i=0; i<l; ++i) {
		list.push_back (ASTnode (new ASTcallindex (const_cast <ASTcall *> (this), i)));
	}
	return list;
	
}

string ASTcall::toString (const string & indent) const {
	
	string ret = indent + callee->toString () + "(";
	for (const ASTnode & arg : args) {
		ret += arg->toString () + ", ";
	}
	return ret + ")";
	
}


}
}