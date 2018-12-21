#include <ast/call.hpp>
#include <ast/operation.hpp>
#include <ast/passes/typesafetypass.hpp>
#include <utility/report.hpp>


namespace AST {


void TypeSafetyPass::visit (const Call_1 & node) {
	assert (node.getFunc ()->getParams ().size () == 1);
	assert (*node.getFunc ()->getParams () [0] == *node.getArg ()->getType ());
}

void TypeSafetyPass::visit (const Call_2 & node) {
	assert (node.getFunc ()->getParams ().size () == 2);
	assert (*node.getFunc ()->getParams () [0] == *node.getLhs ()->getType ());
	assert (*node.getFunc ()->getParams () [1] == *node.getRhs ()->getType ());
}

void TypeSafetyPass::visit (const Call_n & node) {
	assert (node.getFunc ()->getParams ().size () == node.getArgs ().size ());
	for (size_t i=0, l=node.getArgs ().size (); i<l; ++i) {
		assert (*node.getFunc ()->getParams () [i] == *node.getArgs () [i]->getType ());
	}
}

void TypeSafetyPass::visit (const Basic_Operation & node) {
	switch (node.getOpp ()) {
		case parser::OPERATION::ADD:
		case parser::OPERATION::AND:
			assert (node.getArgs ().size () == 2);
			assert (*node.getArgs () [0]->getType () == *node.getArgs () [1]->getType ());
			break;
		case parser::OPERATION::AS:
			break;
		case parser::OPERATION::ASSIGN:
		case parser::OPERATION::ASSIGN_REF:
			assert (node.getArgs ().size () == 2);
			assert (*node.getArgs () [0]->getType () == *Ref::get (node.getArgs () [1]->getType ()));
			break;
		case parser::OPERATION::BAND:
		case parser::OPERATION::BNOT:
		case parser::OPERATION::BOR:
		case parser::OPERATION::BXOR:
		case parser::OPERATION::CMPEQ:
		case parser::OPERATION::CMPGT:
		case parser::OPERATION::CMPGTE:
		case parser::OPERATION::CMPLT:
		case parser::OPERATION::CMPLTE:
		case parser::OPERATION::CMPNE:
			assert (node.getArgs ().size () == 2);
			assert (*node.getArgs () [0]->getType () == *node.getArgs () [1]->getType ());
			break;
		case parser::OPERATION::CONCAT:
			break;
		case parser::OPERATION::DECREMENT:
			assert (node.getArgs ().size () == 1);
			break;
		case parser::OPERATION::DIV:
			assert (node.getArgs ().size () == 2);
			assert (*node.getArgs () [0]->getType () == *node.getArgs () [1]->getType ());
			break;
		case parser::OPERATION::IN:
			break;
		case parser::OPERATION::INCREMENT:
			assert (node.getArgs ().size () == 1);
			break;
		case parser::OPERATION::INDEX:
			break;
		case parser::OPERATION::LNOT:
			assert (node.getArgs ().size () == 1);
			break;
		case parser::OPERATION::MUL:
			assert (node.getArgs ().size () == 2);
			assert (*node.getArgs () [0]->getType () == *node.getArgs () [1]->getType ());
			break;
		case parser::OPERATION::NEG:
			assert (node.getArgs ().size () == 1);
			break;
		case parser::OPERATION::NONE:
			break;
		case parser::OPERATION::OR:
		case parser::OPERATION::REM:
		case parser::OPERATION::SUB:
			assert (node.getArgs ().size () == 2);
			assert (*node.getArgs () [0]->getType () == *node.getArgs () [1]->getType ());
			break;
		case parser::OPERATION::__COUNT__:
			break;
	}
}


}
