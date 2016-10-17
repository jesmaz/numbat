#include <nir/scope.hpp>
#include <parse/tree/base.hpp>
#include <utility/report.hpp>


namespace parser {


const BasicArray <ParseTreeNode *> ParseTreeNode::defaultArgs;
const string ParseTreeNode::defaultStr;

const nir::Instruction * ParseTreeNode::build (nir::Scope * scope) {
	report::logMessage (report::ERROR, scope->getSourceFile (), pos, string ("virtual 'build' not implemented for: ") + typeid (*this).name ());
	return nullptr;
}

const nir::Instruction * ParseTreeNode::buildAllocate (nir::Scope * scope, const string &) {
	report::logMessage (report::ERROR, scope->getSourceFile (), pos, string (typeid (*this).name ()) + " can't create an allocate instruction");
	return nullptr;
}

const nir::Parameter * ParseTreeNode::buildParameter (nir::Scope * scope) {
	report::logMessage (report::ERROR, scope->getSourceFile (), pos, string (typeid (*this).name ()) + " is not a valid parameter");
	return nullptr;
}

const nir::Type * ParseTreeNode::resolveType (nir::Scope * scope) {
	report::logMessage (report::ERROR, scope->getSourceFile (), pos, string (typeid (*this).name ()) + " can't resolve a type");
	return nullptr;
}

void ParseTreeNode::declare (nir::Scope * scope) {
	report::logMessage (report::ERROR, scope->getSourceFile (), pos, string ("virtual 'declare' not implemented for: ") + typeid (*this).name ());
}


}
