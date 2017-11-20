#include <ast/call.hpp>
#include <ast/context.hpp>
#include <ast/function.hpp>
#include <ast/passes/functionpurity.hpp>
#include <vector>


namespace AST {


void FunctionPurityPass::visit (const Call_0 & node) {
	AnalysisPass::visit (node);
	auto & v = node.getFunc ()->getMetaData () [KEY_FUNCTIONAL];
	if (v != VALUE_TRUE) {
		if (v == VALUE_FALSE or activeFunc->getMetaData () [KEY_FUNCTIONAL] == VALUE_TRUE) {
			activeFunc->getMetaData () [KEY_FUNCTIONAL] = v;
		}
	}
}

void FunctionPurityPass::visit (const Call_1 & node) {
	AnalysisPass::visit (node);
	auto & v = node.getFunc ()->getMetaData () [KEY_FUNCTIONAL];
	if (v != VALUE_TRUE) {
		if (v == VALUE_FALSE or activeFunc->getMetaData () [KEY_FUNCTIONAL] == VALUE_TRUE) {
			activeFunc->getMetaData () [KEY_FUNCTIONAL] = v;
		}
	}
}

void FunctionPurityPass::visit (const Call_2 & node) {
	AnalysisPass::visit (node);
	auto & v = node.getFunc ()->getMetaData () [KEY_FUNCTIONAL];
	if (v != VALUE_TRUE) {
		if (v == VALUE_FALSE or activeFunc->getMetaData () [KEY_FUNCTIONAL] == VALUE_TRUE) {
			activeFunc->getMetaData () [KEY_FUNCTIONAL] = v;
		}
	}
}

void FunctionPurityPass::visit (const Call_n & node) {
	AnalysisPass::visit (node);
	auto & v = node.getFunc ()->getMetaData () [KEY_FUNCTIONAL];
	if (v != VALUE_TRUE) {
		if (v == VALUE_FALSE or activeFunc->getMetaData () [KEY_FUNCTIONAL] == VALUE_TRUE) {
			activeFunc->getMetaData () [KEY_FUNCTIONAL] = v;
		}
	}
}

void FunctionPurityPass::visit (const Function_Ptr & node) {
	AnalysisPass::visit (node);
	auto & v = node.getFunc ()->getMetaData () [KEY_FUNCTIONAL];
	if (v != VALUE_TRUE) {
		if (v == VALUE_FALSE or activeFunc->getMetaData () [KEY_FUNCTIONAL] == VALUE_TRUE) {
			activeFunc->getMetaData () [KEY_FUNCTIONAL] = v;
		}
	}
}

void FunctionPurityPass::visit (const Function_Set & node) {
	AnalysisPass::visit (node);
	for (auto & f : node.getSet ().funcs) {
		auto & v = f.first->getMetaData () [KEY_FUNCTIONAL];
		if (v != VALUE_TRUE) {
			if (v == VALUE_FALSE or activeFunc->getMetaData () [KEY_FUNCTIONAL] == VALUE_TRUE) {
				activeFunc->getMetaData () [KEY_FUNCTIONAL] = v;
			}
		}
	}
}

void FunctionPurityPass::visit (const Variable & node) {
	AnalysisPass::visit (node);
	//TODO: Check to see if the variable is gloabally linked
	//TODO: Make sure global constants can be initalised staticly
}

void FunctionPurityPass::analyse (const std::set <FuncPtr> & funcs) {
	
	std::vector <FuncPtr> pending, failed (funcs.begin (), funcs.end ());
	
	size_t success=1, fail=1;
	while (success and fail) {
		
		success = 0;
		fail = 0;
		std::swap (pending, failed);
		failed.clear ();
		
		for (auto & f : pending) {
			if ((FunctionPurityPass (f))()) {
				++success;
			} else {
				++fail;
				failed.push_back (f);
			}
		}
		
	}
	
	// By process of elimination the remaining functions must be pure
	for (auto & f : failed) {
		f->getMetaData () [KEY_FUNCTIONAL] = VALUE_TRUE;
	}
	
}

bool FunctionPurityPass::operator ()() {
	
	// TODO: check if the function is external
	activeFunc->getMetaData () [KEY_FUNCTIONAL] = VALUE_TRUE;
	if (activeFunc->getBody ()) {
		this->analyse (activeFunc->getBody ());
	}
	return activeFunc->getMetaData () [KEY_FUNCTIONAL] != VALUE_NIL;
	
}


}
