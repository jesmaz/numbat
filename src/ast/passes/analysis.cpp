#include <ast/call.hpp>
#include <ast/context.hpp>
#include <ast/flowcontrol.hpp>
#include <ast/memory.hpp>
#include <ast/meta.hpp>
#include <ast/operation.hpp>
#include <ast/passes/analysis.hpp>
#include <ast/type.hpp>
#include <ast/sequence.hpp>
#include <ast/typecast.hpp>
#include <ast/variable.hpp>


namespace AST {


void AnalysisPass::visit (const And & node) {
	this->analyse (node.getFirst ());
	this->analyse (node.getSecond ());
	this->analyse (node.getType ());
}

void AnalysisPass::visit (const Array & node) {
	this->analyse (node.getBaseType ());
}

void AnalysisPass::visit (const ArrayInit & node) {
	this->analyse (node.getLength ());
	this->analyse (node.getVal ());
}

void AnalysisPass::visit (const Basic_Operation & node) {
	for (auto & arg : node.getArgs ()) {
		this->analyse (arg);
	}
}

void AnalysisPass::visit (const Call_0 & node) {
	this->analyse (node.getType ());
}

void AnalysisPass::visit (const Call_1 & node) {
	this->analyse (node.getArg ());
	this->analyse (node.getType ());
}

void AnalysisPass::visit (const Call_2 & node) {
	this->analyse (node.getLhs ());
	this->analyse (node.getRhs ());
	this->analyse (node.getType ());
}

void AnalysisPass::visit (const Call_n & node) {
	for (auto & arg : node.getArgs ()) {
		this->analyse (arg);
	}
	this->analyse (node.getType ());
}

void AnalysisPass::visit (const CastToArbPrec & node) {
	this->analyse (node.getNode ());
	this->analyse (node.getType ());
}

void AnalysisPass::visit (const CastToDecimal & node) {
	this->analyse (node.getNode ());
	this->analyse (node.getType ());
}

void AnalysisPass::visit (const CastToFloat & node) {
	this->analyse (node.getNode ());
	this->analyse (node.getType ());
}

void AnalysisPass::visit (const CastToInt & node) {
	this->analyse (node.getNode ());
	this->analyse (node.getType ());
}

void AnalysisPass::visit (const CastToUint & node) {
	this->analyse (node.getNode ());
	this->analyse (node.getType ());
}

void AnalysisPass::visit (const Const & node) {
	this->analyse (node.getRegType ());
}

void AnalysisPass::visit (const Function_Ptr & node) {
	this->analyse (node.getType ());
}

void AnalysisPass::visit (const Function_Set & node) {
	this->analyse (node.getType ());
}

void AnalysisPass::visit (const IfElse & node) {
	this->analyse (node.getCond ());
	this->analyse (node.getBody ());
	this->analyse (node.getAlt ());;
	this->analyse (node.getVar ());
}

void AnalysisPass::visit (const Inferred &) {}

void AnalysisPass::visit (const Interface &) {}

void AnalysisPass::visit (const Numeric &) {}

void AnalysisPass::visit (const Or & node) {
	this->analyse (node.getFirst ());
	this->analyse (node.getSecond ());
	this->analyse (node.getType ());
}

void AnalysisPass::visit (const RawInit & node) {
	this->analyse (node.getVar ());
	for (auto & arg : node.getArgs ()) {
		this->analyse (arg);
	}
}

void AnalysisPass::visit (const Ref & node) {
	this->analyse (node.getRegType ());
}

void AnalysisPass::visit (const Reflect & node) {
	this->analyse (node.getMetaTag ());
	this->analyse (node.getTarget ());
	this->analyse (node.getType ());
}

void AnalysisPass::visit (const ReflectType & node) {
	this->analyse (node.getMetaTag ());
	this->analyse (node.getTarget ());
	this->analyse (node.getRegType ());
}

void AnalysisPass::visit (const Return & node) {
	this->analyse (node.getRetVal ());
}

void AnalysisPass::visit (const Sequence & node) {
	for (auto & n : node.getNodes ()) {
		this->analyse (n);
	}
}

void AnalysisPass::visit (const StaticIndex & node) {
	this->analyse (node.getParent ());
	this->analyse (node.getType ());
}

void AnalysisPass::visit (const StaticValue & node) {
	this->analyse (node.getType ());
}

void AnalysisPass::visit (const Struct & node) {
	for (auto & t : node.getMembers ()) {
		this->analyse (t);
	}
}

void AnalysisPass::visit (const Unresolved_Call & node) {
	for (auto & arg : node.getArgs ()) {
		this->analyse (arg);
	}
}

void AnalysisPass::visit (const Unresolved_Constructor & node) {
	for (auto & arg : node.getArgs ()) {
		this->analyse (arg);
	}
	this->analyse (node.getVar ());
}

void AnalysisPass::visit (const Unresolved_Get_Member & node) {
	this->analyse (node.getParent ());
}

void AnalysisPass::visit (const Unresolved_IfElse & node) {
	this->analyse (node.getCond ());
	this->analyse (node.getBody ());
	this->analyse (node.getAlt ());
}

void AnalysisPass::visit (const Unresolved_Operation & node) {
	for (auto & arg : node.getArgs ()) {
		this->analyse (arg);
	}
}

void AnalysisPass::visit (const Variable & node) {
	this->analyse (node.getType ());
}


}
