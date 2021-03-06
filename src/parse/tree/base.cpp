#include <parse/tree/base.hpp>
#include <utility/report.hpp>

const BasicArray <ParseTreeNode *> ParseTreeNode::defaultArgs;
const string ParseTreeNode::defaultStr;

const nir::Instruction * ParseTreeNode::build (nir::Scope *) {
	report::logMessage (report::ERROR, "", line, pos, string ("virtual 'build' not implemented for: ") + typeid (*this).name ());
	return nullptr;
}

const nir::Instruction * ParseTreeNode::buildAllocate (nir::Scope *, const string &) {
	report::logMessage (report::ERROR, "", line, pos, string (typeid (*this).name ()) + " can't create an allocate instruction");
	return nullptr;
}

const nir::Instruction * ParseTreeNode::buildParameter (nir::Scope *) {
	report::logMessage (report::ERROR, "", line, pos, string (typeid (*this).name ()) + " is not a valid parameter");
	return nullptr;
}

const nir::Type * ParseTreeNode::resolveType (nir::Scope *) {
	report::logMessage (report::ERROR, "", line, pos, string (typeid (*this).name ()) + " can't resolve a type");
	return nullptr;
}

void ParseTreeNode::declare (nir::Scope *) {
	report::logMessage (report::ERROR, "", line, pos, string ("virtual 'declare' not implemented for: ") + typeid (*this).name ());
}

void ParseTreeNode::push_back (ParseTreeNode * e) {
	report::logMessage (report::ERROR, "", line, pos, string ("virtual 'push_back' not implemented for: ") + typeid (*this).name ());
}
