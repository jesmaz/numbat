#include <ast/call.hpp>
#include <ast/flowcontrol.hpp>
#include <ast/function.hpp>
#include <ast/node.hpp>
#include <ast/literal.hpp>
#include <ast/operation.hpp>
#include <ast/passes/nir.hpp>
#include <ast/sequence.hpp>
#include <ast/variable.hpp>
#include <map>
#include <nir/instruction.hpp>
#include <nir/module.hpp>
#include <nir/parameter.hpp>
#include <nir/type/array.hpp>
#include <nir/type/number.hpp>
#include <nir/scope.hpp>
#include <utility/report.hpp>


namespace AST {


std::map <const Function *, const nir::Function *> functionMap;
std::map <const Type *, const nir::Type *> typeMap;
std::map <const Variable *, const nir::Instruction *> variableMap;


const nir::Function * resolve (nir::Scope * scope, const Function * func);
const nir::Type * resolve (nir::Scope * scope, const Type * type);


const nir::Function * resolve (nir::Scope * scope, const Function * func) {
	auto itt = functionMap.find (func);
	if (itt != functionMap.end ()) {
		return itt->second;
	} else {
		auto params = func->getParams ().map <const nir::Parameter *> ([&](auto & n) {
			return new nir::Parameter (resolve (scope, n.get ()), *(scope->getModule ()->newSymbol ("")));
		});
		auto retTypes = func->getRetVals ().map <const nir::Parameter *> ([&](auto & n) {
			return new nir::Parameter (resolve (scope, n.get ()), *(scope->getModule ()->newSymbol ("")));
		});
		
		auto * s = scope->declareFunction (params, retTypes, func->getIden (), nir::LINKAGE::LOCAL);
		return functionMap [func] = s->getOwner ();
	}
}


const nir::Type * resolveType (nir::Scope * scope, const Node * node) {
	assert (typeid (*node) == typeid (Type));
	return resolve (scope, reinterpret_cast <const Type *> (node));
}

const nir::Type * resolve (nir::Scope * scope, const Type * type) {
	auto itt = typeMap.find (type);
	if (itt != typeMap.end ()) {
		return itt->second;
	} else {
		return typeMap [type] = NirTypePass (scope).transform (type);
	}
}


void NirPass::visit (const Basic_Operation & node) {
	
	auto args = node.getArgs ().map <nir::Argument> ([&](auto & n) {
		auto * inst = this->transform (n);
		return nir::Argument (inst, inst->getIden ());
	});
	
	inst = nullptr;
	switch (node.getOpp ()) {
		case parser::OPERATION::ADD:
			assert (args.size () == 2);
			inst = scope->createAdd (args [0], args [1]);
			break;
		case parser::OPERATION::AND: {
			assert (args.size () == 2);
			
			const nir::Type * b = scope->resolveType ("bool", node.getPos ());
			const nir::Instruction * res = scope->allocateVariable (b);
			
			nir::symbol rhsBlock = scope->createBlock ();
			nir::symbol cont = scope->createBlock ();
			
			scope->createPut (args [0], {res, res->getIden ()});
			const nir::Instruction * cond = scope->createBitNot (args [0]);
			scope->createJump ({cond, cond->getIden ()}, cont);
			scope->changeActiveBlock (rhsBlock);
			
			scope->createPut (args [1], {res, res->getIden ()});
			scope->changeActiveBlock (cont);
			
			inst = scope->createGet ({res, res->getIden ()});
			break;
		}
		case parser::OPERATION::AS:
			assert (args.size () == 2);
			inst = scope->createReinterpret (args [0], resolveType (scope, node.getArgs () [1].get ()));
			break;
		case parser::OPERATION::ASSIGN:
			assert (args.size () == 2);
			inst = scope->createAssign (args [0], args [1]);
			break;
		case parser::OPERATION::BAND:
			assert (args.size () == 2);
			inst = scope->createBitAnd (args [0], args [1]);
			break;
		case parser::OPERATION::BNOT:
			assert (args.size () == 1);
			inst = scope->createBitNot (args [0]);
			break;
		case parser::OPERATION::BOR:
			assert (args.size () == 2);
			inst = scope->createBitOr (args [0], args [1]);
			break;
		case parser::OPERATION::BXOR:
			assert (args.size () == 2);
			inst = scope->createBitXor (args [0], args [1]);
			break;
		case parser::OPERATION::CONCAT: {
			assert (args.size () == 2);
			auto * lhsType = args [0].type->getValueType ();
			auto * rhsType = args [1].type->getValueType ();
			if (typeid (*lhsType) == typeid (nir::Array)) {
				auto * lhsArrType = reinterpret_cast <const nir::Array *> (lhsType);
				
				auto * lhsLen = scope->resolve (args [0], "len", node.getArgs () [0]->getPos ());
				auto * rhsLen = lhsLen;
				
				if (lhsType == rhsType) {
					auto * rhsLen = scope->resolve (args [1], "len", node.getArgs () [1]->getPos ());
					
				} else if (lhsType == nir::Array::arrayOf (rhsType)) {
					rhsLen = scope->createConstant (lhsLen->getType (), "1", node.getArgs () [0]->getPos ());
					
				} else {
					report::logMessage (report::ERROR, scope->getSourceFile (), node.getPos (), "Can't concatenate '" + node.getArgs () [0]->toString (text::PLAIN) + "' to '" + node.getArgs () [0]->toString (text::PLAIN) + "', they have incompatible types");
					inst = nullptr;
					break;
					
				}
				
				auto * len = scope->createAdd (lhsLen, rhsLen);
				auto * newArr = scope->allocateArray (lhsArrType->getElementType (), len);
				auto dest1 = scope->loadReference (scope->resolve (newArr, "data", node.getPos ()));
				auto src1 = scope->loadReference (scope->resolve (args [0], "data", node.getArgs () [0]->getPos ()));
				scope->createCopy (dest1, src1, lhsLen);
				auto dest2 = scope->createPointerAdd (dest1.type, dest1, lhsLen);
				auto src2 = scope->loadReference (scope->resolve (args [1], "data", node.getArgs () [1]->getPos ()));
				scope->createCopy (dest2, src2, rhsLen);
				
				inst = newArr;
				
			}
			break;
		}
		case parser::OPERATION::DECREMENT:
			abort ();
			break;
		case parser::OPERATION::DIV:
			assert (args.size () == 2);
			inst = scope->createDiv (args [0], args [1]);
			break;
		case parser::OPERATION::CMPEQ:
			assert (args.size () == 2);
			inst = scope->createCmpEQ (args [0], args [1]);
			break;
		case parser::OPERATION::CMPGT:
			assert (args.size () == 2);
			inst = scope->createCmpGT (args [0], args [1]);
			break;
		case parser::OPERATION::CMPGTE:
			assert (args.size () == 2);
			inst = scope->createCmpGTE (args [0], args [1]);
			break;
		case parser::OPERATION::CMPLT:
			assert (args.size () == 2);
			inst = scope->createCmpLT (args [0], args [1]);
			break;
		case parser::OPERATION::CMPLTE:
			assert (args.size () == 2);
			inst = scope->createCmpLTE (args [0], args [1]);
			break;
		case parser::OPERATION::CMPNE:
			assert (args.size () == 2);
			inst = scope->createCmpNE (args [0], args [1]);
			break;
		case parser::OPERATION::IN:
			abort ();
			break;
		case parser::OPERATION::INCREMENT:
			abort ();
			break;
		case parser::OPERATION::INDEX: {
			assert (args.size () >= 2);
			auto * dat = scope->resolve (args [0], "data", node.getPos ());
			auto ptr = scope->createGet ({dat, dat->getIden ()});
			inst = scope->createPointerAdd (ptr->getType (), {ptr, ptr->getIden ()}, args [1], "index");
			break;
		}
		case parser::OPERATION::LNOT:
			assert (args.size () == 1);
			inst = scope->createLNot (args [0]);
			break;
		case parser::OPERATION::MUL:
			assert (args.size () == 2);
			inst = scope->createMul (args [0], args [1]);
			break;
		case parser::OPERATION::NEG:
			assert (args.size () == 1);
			inst = scope->createNeg (args [0]);
			break;
		case parser::OPERATION::NONE:
			abort ();
			break;
		case parser::OPERATION::OR:{
			assert (args.size () == 2);
			
			const nir::Type * b = scope->resolveType ("bool", node.getPos ());
			const nir::Instruction * res = scope->allocateVariable (b);
			
			nir::symbol rhsBlock = scope->createBlock ();
			nir::symbol cont = scope->createBlock ();
			
			scope->createPut (args [0], {res, res->getIden ()});
			const nir::Instruction * cond = scope->createBitNot (args [0]);
			cond = scope->createLNot ({cond, cond->getIden ()});
			scope->createJump ({cond, cond->getIden ()}, cont);
			scope->changeActiveBlock (rhsBlock);
			
			scope->createPut (args [1], {res, res->getIden ()});
			scope->changeActiveBlock (cont);
			
			inst = scope->createGet ({res, res->getIden ()});
			break;
		}
		case parser::OPERATION::REM:
			assert (args.size () == 2);
			inst = scope->createRem (args [0], args [1]);
			break;
		case parser::OPERATION::SUB:
			assert (args.size () == 2);
			inst = scope->createSub (args [0], args [1]);
			break;
		case parser::OPERATION::__COUNT__:
			abort ();
			break;
	}
}

void NirPass::visit (const Call_0 & node) {
	inst = scope->createCall (resolve (scope, node.getFunc ().get ()), {});
}

void NirPass::visit (const Call_1 & node) {
	auto * arg = this->transform (node.getArg ());
	inst = scope->createCall (resolve (scope, node.getFunc ().get ()), {{arg, arg->getIden ()}});
}

void NirPass::visit (const Call_2 & node) {
	auto * lhs = this->transform (node.getLhs ());
	auto * rhs = this->transform (node.getRhs ());
	inst = scope->createCall (resolve (scope, node.getFunc ().get ()), {{lhs, lhs->getIden ()}, {rhs, rhs->getIden ()}});
}

void NirPass::visit (const Call_n & node) {
	inst = scope->createCall (resolve (scope, node.getFunc ().get ()), node.getArgs ().map <nir::Argument> ([&](auto & arg) {
		auto * inst = this->transform (arg);
		return nir::Argument (inst, inst->getIden ());
	}));
}

void NirPass::visit (const IfElse & node) {
	auto v = transform (node.getVar ());
	const nir::Instruction * condition = transform (node.getCond ());
	
	const nir::Instruction * bodyRet=nullptr, * altRet=nullptr;
	nir::Block * current=scope->getCurrentBlock ();
	nir::symbol bodyBlock=scope->createBlock (), altBlock=nullptr;
	
	if (node.getAlt ()) {
		altBlock=scope->createBlock ();
	}
	
	nir::symbol contBlock=scope->createBlock ();
	
	// Build main body of if statement
	scope->changeActiveBlock (bodyBlock);
	bodyRet = transform (node.getBody ());
	
	if (altBlock) {
		// Build main body of else statement
		scope->changeActiveBlock (altBlock);
		altRet = transform (node.getAlt ());
	}
	
	// Go back to before the if statement to allocate and branch
	scope->changeActiveBlock (current);
	
	scope->createJump ({condition, condition->getIden ()}, bodyBlock);
	
	
	if (altBlock) {
		scope->createJump (altBlock);
	} else {
		scope->createJump (contBlock);
	}
	
	scope->changeActiveBlock (bodyBlock);
	scope->createJump (contBlock);
	scope->changeActiveBlock (contBlock);
	
	inst = v;
}

void NirPass::visit (const Number & node) {
	auto pos = node.getPos ();
	auto literal = node.getValue ()->toString (text::PrintMode::PLAIN);
	const nir::Type * t;
	switch (static_cast <const Numeric *> (node.getType ().get ())->getArith ()) {
		case Numeric::ArithmaticType::UNDETERMINED:
		case Numeric::ArithmaticType::ARBITRARY:
		case Numeric::ArithmaticType::DECINT:
			abort ();
		case Numeric::ArithmaticType::FPINT:
			t = scope->resolveType ("double", pos);
			break;
		case Numeric::ArithmaticType::INT:
			t = scope->resolveType ("int64", pos);
			break;
		case Numeric::ArithmaticType::UINT:
			t = scope->resolveType ("uint64", pos);
			break;
	}
	inst = scope->createConstant (t, literal, pos);
}

void NirPass::visit (const Sequence & seq) {
	for (auto & node : seq.getNodes ()) {
		inst = this->transform (node);
	}
	if (not inst) {
		inst = scope->createConstant (scope->resolveType ("bool", seq.getPos ()), "0", seq.getPos ());
	}
}

void NirPass::visit (const Variable & node) {
	auto itt = variableMap.find (&node);
	if (itt != variableMap.end ()) {
		inst = itt->second;
	} else {
		inst = variableMap [&node] = scope->allocateVariable (resolve (scope, node.getType ().get ()), node.getIden ());
	}
}


const nir::Type * NirTypePass::transform (const Type * t) {
	type = nullptr;
	t->accept (*this);
	assert (type);
	return type;
}

void NirTypePass::visit (const Array & node) {
	type = nir::Array::arrayOf (resolve (scope, node.getBaseType ().get ()));
}

void NirTypePass::visit (const Const & node) {
	type = resolve (scope, node.getRegType ().get ());
}

void NirTypePass::visit (const Inferred & node) {
	abort ();
}

void NirTypePass::visit (const Interface & node) {
	abort ();
}

void NirTypePass::visit (const Numeric & node) {
	switch (node.getArith ()) {
		case Numeric::ArithmaticType::UNDETERMINED:
		case Numeric::ArithmaticType::ARBITRARY:
		case Numeric::ArithmaticType::DECINT:
			//TODO: Replace with decimal int
			type = nir::Number::getNumberType (nir::Number::ArithmaticType::FPINT, 64);
			break;
		case Numeric::ArithmaticType::INT:
			type = nir::Number::getNumberType (nir::Number::ArithmaticType::INT, node.getMinPrec ());
			break;
		case Numeric::ArithmaticType::FPINT:
			type = nir::Number::getNumberType (nir::Number::ArithmaticType::FPINT, node.getMinPrec ());
			break;
		case Numeric::ArithmaticType::UINT:
			type = nir::Number::getNumberType (nir::Number::ArithmaticType::UINT, node.getMinPrec ());
			break;
	}
}

void NirTypePass::visit (const Ref & node) {
	type = resolve (scope, node.getRegType ().get ())->getPointerTo ();
}

void NirTypePass::visit (const Struct & node) {
	type = nir::Tuple::getTuple (node.getMembers ().map <const nir::Type *> ([&](auto & n) {
		return resolve (scope, n->getType ().get ());
	}));
}


}

