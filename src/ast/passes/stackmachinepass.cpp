#include <ast/call.hpp>
#include <ast/flowcontrol.hpp>
#include <ast/function.hpp>
#include <ast/memory.hpp>
#include <ast/operation.hpp>
#include <ast/passes/analysis.hpp>
#include <ast/passes/stackmachine.hpp>
#include <ast/sequence.hpp>
#include <ast/variable.hpp>


namespace AST {


void StackMachinePass::visit (const Basic_Operation & node) {
	
	switch (node.getOpp ()) {
		case parser::OPERATION::ASSIGN: {
			push (node.getArgs () [0]);
			push ({stackmachine::OP_CODE::DUPLICATE, -1});
			load (node.getArgs () [1]);
			push ({stackmachine::OP_CODE::COPY, getLayout (node.getArgs () [1]->getType ())});
			break;
		}
		case parser::OPERATION::ADD: {
			load (node.getArgs () [0]);
			load (node.getArgs () [1]);
			push (stackmachine::OP_CODE::OP_ADD);
			break;
		}
		case parser::OPERATION::CMPEQ: {
			load (node.getArgs () [0]);
			load (node.getArgs () [1]);
			push (stackmachine::OP_CODE::OP_CMP_EQ);
			break;
		}
		case parser::OPERATION::CMPGT: {
			load (node.getArgs () [0]);
			load (node.getArgs () [1]);
			push (stackmachine::OP_CODE::OP_CMP_GT);
			break;
		}
		case parser::OPERATION::CMPGTE: {
			load (node.getArgs () [0]);
			load (node.getArgs () [1]);
			push (stackmachine::OP_CODE::OP_CMP_GTE);
			break;
		}
		case parser::OPERATION::CMPLT: {
			load (node.getArgs () [0]);
			load (node.getArgs () [1]);
			push (stackmachine::OP_CODE::OP_CMP_LT);
			break;
		}
		case parser::OPERATION::CMPLTE: {
			load (node.getArgs () [0]);
			load (node.getArgs () [1]);
			push (stackmachine::OP_CODE::OP_CMP_LTE);
			break;
		}
		case parser::OPERATION::CMPNE: {
			load (node.getArgs () [0]);
			load (node.getArgs () [1]);
			push (stackmachine::OP_CODE::OP_CMP_NE);
			break;
		}
		case parser::OPERATION::CONCAT: {
			auto base = dynamic_cast <const Array*> (node.getType ().get ())->getBaseType ();
			
			load (node.getArgs () [0]);
			// [arg0.data*][arg0.len]
			load (node.getArgs () [1]);
			// [arg0.data*][arg0.len][arg1.data*][arg1.len]
			push ({stackmachine::OP_CODE::DUPLICATE, -3});
			// [arg0.data*][arg0.len][arg1.data*][arg1.len][arg0.len]
			push ({stackmachine::OP_CODE::DUPLICATE, -2});
			// [arg0.data*][arg0.len][arg1.data*][arg1.len][arg0.len][arg1.len]
			push ({stackmachine::OP_CODE::OP_ADD});
			// [arg0.data*][arg0.len][arg1.data*][arg1.len][len]
			push ({stackmachine::OP_CODE::LOAD_GLOBAL_ADDR, symbol_t ("malloc")});
			// [arg0.data*][arg0.len][arg1.data*][arg1.len][len][malloc*]
			push ({stackmachine::OP_CODE::DUPLICATE, -2});
			// [arg0.data*][arg0.len][arg1.data*][arg1.len][len][malloc*][len]
			push ({stackmachine::OP_CODE::SIZE_OF, getLayout (base)});
			// [arg0.data*][arg0.len][arg1.data*][arg1.len][len][malloc*][len][size]
			push ({stackmachine::OP_CODE::OP_MUL});
			// [arg0.data*][arg0.len][arg1.data*][arg1.len][len][malloc*][bytes]
			push ({stackmachine::OP_CODE::CALL, symbol_t ("malloc")});
			// [arg0.data*][arg0.len][arg1.data*][arg1.len][len][data*]
			
			push ({stackmachine::OP_CODE::DUPLICATE, -5});
			// [arg0.data*][arg0.len][arg1.data*][arg1.len][len][data*][arg0.len]
			push ({stackmachine::OP_CODE::SIZE_OF, getLayout (base)});
			// [arg0.data*][arg0.len][arg1.data*][arg1.len][len][data*][arg0.len][size]
			push ({stackmachine::OP_CODE::OP_MUL});
			// [arg0.data*][arg0.len][arg1.data*][arg1.len][len][data*][arg0bytes]
			push ({stackmachine::OP_CODE::DUPLICATE, -2});
			// [arg0.data*][arg0.len][arg1.data*][arg1.len][len][data*][arg0bytes][data*]
			push ({stackmachine::OP_CODE::DUPLICATE, -8});
			// [arg0.data*][arg0.len][arg1.data*][arg1.len][len][data*][arg0bytes][data*][arg0.data*]
			push ({stackmachine::OP_CODE::DUPLICATE, -3});
			// [arg0.data*][arg0.len][arg1.data*][arg1.len][len][data*][arg0bytes][data*][arg0.data*][arg0bytes]
			push ({stackmachine::OP_CODE::COPY_MEMCOPY});
			// [arg0.data*][arg0.len][arg1.data*][arg1.len][len][data*][arg0bytes]
			
			push ({stackmachine::OP_CODE::DUPLICATE, -2});
			// [arg0.data*][arg0.len][arg1.data*][arg1.len][len][data*][arg0bytes][data*]
			push ({stackmachine::OP_CODE::OP_ADD});
			// [arg0.data*][arg0.len][arg1.data*][arg1.len][len][data*][middata*]
			push ({stackmachine::OP_CODE::DUPLICATE, -5});
			// [arg0.data*][arg0.len][arg1.data*][arg1.len][len][data*][middata*][arg1.data*]
			push ({stackmachine::OP_CODE::DUPLICATE, -5});
			// [arg0.data*][arg0.len][arg1.data*][arg1.len][len][data*][middata*][arg1.data*][arg1.len]
			push ({stackmachine::OP_CODE::SIZE_OF, getLayout (base)});
			// [arg0.data*][arg0.len][arg1.data*][arg1.len][len][data*][middata*][arg1.data*][arg1.len][size]
			push ({stackmachine::OP_CODE::OP_MUL});
			// [arg0.data*][arg0.len][arg1.data*][arg1.len][len][data*][middata*][arg1.data*][arg1bytes]
			push ({stackmachine::OP_CODE::COPY_MEMCOPY});
			// [arg0.data*][arg0.len][arg1.data*][arg1.len][len][data*]
			
			push ({stackmachine::OP_CODE::MOVE, -5});
			// [data*][arg0.len][arg1.data*][arg1.len][len]
			push ({stackmachine::OP_CODE::MOVE, -3});
			// [data*][len][arg1.data*][arg1.len]
			push ({stackmachine::OP_CODE::POP, 2});
			// [data*][len]
			
			break;
		}
		case parser::OPERATION::DIV: {
			load (node.getArgs () [0]);
			load (node.getArgs () [1]);
			push (stackmachine::OP_CODE::OP_DIV);
			break; 
		}
		case parser::OPERATION::LNOT: {
			load (node.getArgs () [0]);
			push (stackmachine::OP_CODE::OP_NOT);
			break;
		}
		case parser::OPERATION::MUL: {
			load (node.getArgs () [0]);
			load (node.getArgs () [1]);
			push (stackmachine::OP_CODE::OP_MUL);
			break;
		}
		case parser::OPERATION::SUB: {
			load (node.getArgs () [0]);
			load (node.getArgs () [1]);
			push (stackmachine::OP_CODE::OP_SUB);
			break;
		}
		default:
			abort ();
	}
	
}

void StackMachinePass::visit (const Call_0 & node) {abort ();}

void StackMachinePass::visit (const Call_1 & node) {
	if (node.getFunc ()->getNative () != nullptr) {
		auto id = "native_" + node.getFunc ()->getIden ();
		if (c.globals.find (id) == c.globals.end ()) {
			c.globals [id] = std::make_pair (getLayout (node.getType ()), Literal (node.getFunc ()->getNative ()));
		}
	}
	
	load (node.getArg ());
	push ({stackmachine::OP_CODE::LOAD_GLOBAL_ADDR, node.getFunc ()->getIden ()});
	push ({stackmachine::OP_CODE::CALL, getLayout (node.getFunc ())});
}

void StackMachinePass::visit (const Call_2 & node) {abort ();}

void StackMachinePass::visit (const Call_n & node) {
	if (node.getFunc ()->getNative () != nullptr) {
		auto id = "native_" + node.getFunc ()->getIden ();
		if (c.globals.find (id) == c.globals.end ()) {
			c.globals [id] = std::make_pair (getLayout (node.getType ()), Literal (node.getFunc ()->getNative ()));
		}
	}
	
	for (auto & a : node.getArgs ()) {
		load (a);
	}
	push ({stackmachine::OP_CODE::LOAD_GLOBAL_ADDR, node.getFunc ()->getIden ()});
	push ({stackmachine::OP_CODE::CALL, getLayout (node.getFunc ())});
}

void StackMachinePass::visit (const Function_Ptr & node) {
	push ({stackmachine::OP_CODE::LOAD_GLOBAL_ADDR, node.getFunc ()->getIden ()});
}

void StackMachinePass::visit (const IfElse & node) {
	
	auto endLabel = std::to_string (size_t (&node)) + "_continue";
	
	load (node.getCond ());
	push ({stackmachine::OP_CODE::OP_NOT});
	if (node.getAlt ()) {
		auto altLabel = std::to_string (size_t (&node)) + "_alt";
		push ({stackmachine::OP_CODE::JMP_IF, altLabel});
		push (node.getBody ());
		push ({stackmachine::OP_CODE::JMP, endLabel});
		push ({stackmachine::OP_CODE::LABEL, altLabel});
		push (node.getAlt ());
	} else {
		push ({stackmachine::OP_CODE::JMP_IF, endLabel});
		push (node.getBody ());
	}
	push ({stackmachine::OP_CODE::LABEL, endLabel});
	
}

void StackMachinePass::visit (const RawInit & node) {
	push (node.getVar ());
	push ({stackmachine::OP_CODE::DUPLICATE, -1});
	for (auto & a : node.getArgs ()) {
		load (a);
	}
	push ({stackmachine::OP_CODE::COPY, getLayout (node.getType ())});
}

void StackMachinePass::visit (const Sequence & node) {
	
	size_t top = c.tracker.getStackSize ();
	for (auto v : node.getLocalStack ()) {
		if (v->getLocation () == Variable::LOCATION::LOCAL) {
			if (c.stackVariables.find (v.get ()) == c.stackVariables.end ()) {
				c.stackVariables [v.get ()] = c.push ({stackmachine::OP_CODE::RESERVE, c.getLayout (v->getType ())});
				c.push ({stackmachine::OP_CODE::LABEL, symbol_t ("Allocating space for " + v->getIden () + " at offset " + std::to_string (c.stackVariables [v.get ()]))});
			} else {
				c.push ({stackmachine::OP_CODE::LABEL, symbol_t ("Already allocated space for " + v->getIden () + " at offset " + std::to_string (c.stackVariables [v.get ()]))});
			}
		}
	}
	
	if (node.getNodes ().empty ()) {
		size = 0;
		
	} else {
		
		for (size_t i=0, l=node.getNodes ().size ()-1; i<l; ++i) {
			push (node.getNodes () [i]);
			//push ({stackmachine::OP_CODE::POP, int (c.tracker.getStackSize () - top)});
		}
		
		push (node.getNodes ().back ());
		
	}
	
}

void StackMachinePass::visit (const StaticIndex & node) {
	
	push (node.getParent ());
	if (node.getParent ()->getType ()->isRef ()) {
		auto layout = getLayout (node.getParent ()->getType ());
		push ({stackmachine::OP_CODE::LOAD, layout});
	}
	push ({stackmachine::OP_CODE::OFFSET, int (node.getIndex ()), getLayout (node.getParent ()->getType ())});
	
}

void StackMachinePass::visit (const StaticValue & node) {
	
	auto layout = getLayout (node.getType ());
	Literal l = node.getLiteral ();
	assert (l == node.getLiteral ());
	auto id = "literal_" + *layout + "_" + node.getLiteral ().toString (text::PrintMode::PLAIN);
	assert (l == node.getLiteral ());
	if (c.globals.find (id) == c.globals.end ()) {
		c.globals [id] = std::make_pair (layout, node.getLiteral ());
	}
	assert (l == node.getLiteral ());
	assert (l == c.globals [id].second);
	push ({stackmachine::OP_CODE::LOAD_GLOBAL_ADDR, id});
	
}

void StackMachinePass::visit (const SystemCall & node) {
	
	for (auto & arg : node.getArgs ()) {
		load (arg);
	}
	push ({stackmachine::OP_CODE::CALL_SYS, node.getSysName ()});
	
}

void StackMachinePass::visit (const Variable & node) {
	
	stackmachine::Instruction inst;
	if (node.getLocation () == Variable::LOCATION::GLOBAL) {
		auto iden = "global_" + node.getIden () + "_" + std::to_string (size_t (&node));
		auto itt = c.globals.find (iden);
		if (itt == c.globals.end ()) {
			c.globals [iden] = std::make_pair (getLayout (node.getType ()), Literal ());
		}
		push ({stackmachine::OP_CODE::LOAD_GLOBAL_ADDR, iden});
		
	} else {
		if (c.stackVariables.find (&node) == c.stackVariables.end ()) {
			c.stackVariables [&node] = -__INT32_MAX__;
		}
		push ({stackmachine::OP_CODE::LOAD_STACK_ADDR, c.stackVariables [&node]});
		
	}
	
}


int StackMachinePass::load (const NodePtr & node) {
	StackMachineLoadPass pass (c);
	node->accept (pass);
	return pass.size;
}

int StackMachinePass::push (const NodePtr & node) {
	StackMachinePass pass (c);
	node->accept (pass);
	return pass.size;
}

int StackMachinePass::Chunk::push (const stackmachine::Instruction & i) {
	inst.push_back (i);
	return tracker.feed (i);
}


class StackTypePass : public ShallNotPass {
	
	public:
		
		virtual NodePtr visit (const NodePtr &) {abort ();}
		
		virtual void visit (const Array &) {
			layout.push_back (stackmachine::TYPE::_META_STRUCT_BEGIN);
			layout.push_back (stackmachine::TYPE::usize);
			layout.push_back (stackmachine::TYPE::usize);
			layout.push_back (stackmachine::TYPE::_META_STRUCT_END);
		}
		
		virtual void visit (const Const & node) {
			node.getRegType ()->accept (*this);
		}
		
		virtual void visit (const Import & node) {
			layout.push_back (stackmachine::TYPE::usize);
		}
		
		virtual void visit (const Numeric & node) {
			switch (node.getArith ()) {
				case Numeric::ArithmaticType::ARBITRARY:
				case Numeric::ArithmaticType::DECINT:
					abort ();
				case Numeric::ArithmaticType::FPINT:
					if (node.getMinPrec () <= 16) {
						layout.push_back (stackmachine::TYPE::f16);
					} else if (node.getMinPrec () <= 32) {
						layout.push_back (stackmachine::TYPE::f32);
					} else {
						layout.push_back (stackmachine::TYPE::f64);
					}
					break;
				case Numeric::ArithmaticType::INT:
					if (node.getMinPrec () <= 8) {
						layout.push_back (stackmachine::TYPE::i8);
					} else if (node.getMinPrec () <= 16) {
						layout.push_back (stackmachine::TYPE::i16);
					} else if (node.getMinPrec () <= 32) {
						layout.push_back (stackmachine::TYPE::i32);
					} else {
						layout.push_back (stackmachine::TYPE::i64);
					}
					break;
				case Numeric::ArithmaticType::UINT:
					if (node.getMinPrec () <= 8) {
						layout.push_back (stackmachine::TYPE::u8);
					} else if (node.getMinPrec () <= 16) {
						layout.push_back (stackmachine::TYPE::u16);
					} else if (node.getMinPrec () <= 32) {
						layout.push_back (stackmachine::TYPE::u32);
					} else {
						layout.push_back (stackmachine::TYPE::u64);
					}
					break;
				case Numeric::ArithmaticType::UNDETERMINED:
					layout.push_back (stackmachine::TYPE::f64);
					break;
			}
		}
		
		virtual void visit (const Ref &) {
			layout.push_back (stackmachine::TYPE::usize);
		}
		
		virtual void visit (const Struct & node) {
			layout.push_back (stackmachine::TYPE::_META_STRUCT_BEGIN);
			for (auto n : node.getMembers ()) {
				n->getType ()->accept (*this);
			}
			layout.push_back (stackmachine::TYPE::_META_STRUCT_END);
		}
		
		void acceptFunction (const FuncPtr & func) {
			
			for (auto t : func->getParams ()) {
				(*this) (t);
			}
			layout.push_back (stackmachine::TYPE::_META_FUNC_SPLIT);
			for (auto t : func->getRetVals ()) {
				(*this) (t);
			}
			
		}
		
		StackTypePass (DynArray <stackmachine::TYPE> & layout) : layout (layout) {}
		
	protected:
	private:
		
		DynArray <stackmachine::TYPE> & layout;
		
};

symbol_t StackMachinePass::Chunk::getLayout (const FuncPtr & type) {
	
	symbol_t s (type->getIden ());
	auto itt = functionLayouts.find (s);
	if (itt == functionLayouts.end ()) {
		DynArray <symbol_t> params, rets;
		for (auto p : type->getParams ()) {
			params.push_back (getLayout (p));
		}
		for (auto r : type->getRetVals ()) {
			rets.push_back (getLayout (r));
		}
		functionLayouts [s] = std::make_pair (params, rets);
	}
	return s;
	
}

symbol_t StackMachinePass::Chunk::getLayout (const TypePtr & type) {
	
	symbol_t s (type->toString (text::PrintMode::PLAIN));
	auto itt = layouts.find (s);
	if (itt == layouts.end ()) {
		DynArray <stackmachine::TYPE> layout;
		(StackTypePass (layout)) (type);
		layouts [s] = layout;
		tracker.addLayout (s, layout);
	}
	return s;
	
}


void StackMachineLoadPass::visit (const Basic_Operation & node) {
	
	StackMachinePass::visit (node);
	if (node.getOpp () == parser::OPERATION::ASSIGN) {
		push ({stackmachine::OP_CODE::LOAD, getLayout (node.getType ()->getRegType ())});
	}
	
}

void StackMachineLoadPass::visit (const RawInit & node) {
	StackMachinePass::visit (node);
	push ({stackmachine::OP_CODE::LOAD, getLayout (node.getType ())});
}

void StackMachineLoadPass::visit (const Sequence & node) {
	size_t top = c.tracker.getStackSize ();
	for (auto v : node.getLocalStack ()) {
		if (v->getLocation () == Variable::LOCATION::LOCAL) {
			if (c.stackVariables.find (v.get ()) == c.stackVariables.end ()) {
				c.stackVariables [v.get ()] = c.push ({stackmachine::OP_CODE::RESERVE, c.getLayout (v->getType ())});
				c.push ({stackmachine::OP_CODE::LABEL, symbol_t ("Allocating space for " + v->getIden () + " at offset " + std::to_string (c.stackVariables [v.get ()]))});
			} else {
				c.push ({stackmachine::OP_CODE::LABEL, symbol_t ("Already allocated space for " + v->getIden () + " at offset " + std::to_string (c.stackVariables [v.get ()]))});
			}
		}
	}
	
	if (node.getNodes ().empty ()) {
		size = 0;
		
	} else {
		
		for (size_t i=0, l=node.getNodes ().size ()-1; i<l; ++i) {
			push (node.getNodes () [i]);
			//push ({stackmachine::OP_CODE::POP, int (c.tracker.getStackSize () - top)});
		}
		
		load (node.getNodes ().back ());
		
	}
}

void StackMachineLoadPass::visit (const StaticIndex & node) {
	StackMachinePass::visit (node);
	push ({stackmachine::OP_CODE::LOAD, getLayout (node.getType ())});
}

void StackMachineLoadPass::visit (const StaticValue & node) {
	StackMachinePass::visit (node);
	push ({stackmachine::OP_CODE::LOAD, getLayout (node.getType ())});
}

void StackMachineLoadPass::visit (const Variable & node) {
	StackMachinePass::visit (node);
	push ({stackmachine::OP_CODE::LOAD, getLayout (node.getType ())});
}


stackmachine::Chunk stackMachinePass (FuncPtr func) {
	
	if (func->getBody ()) {
		
		StackMachinePass::Chunk c;
		c.inst.push_back ({stackmachine::OP_CODE::LABEL, func->getIden ()});
		for (auto v : func->getInitialStack ()) {
			if (v->getLocation () == Variable::LOCATION::LOCAL) {
				if (c.stackVariables.find (v.get ()) == c.stackVariables.end ()) {
					c.stackVariables [v.get ()] = c.push ({stackmachine::OP_CODE::RESERVE, c.getLayout (v->getType ())});
					c.push ({stackmachine::OP_CODE::LABEL, symbol_t ("Allocating space for " + v->getIden () + " at offset " + std::to_string (c.stackVariables [v.get ()]))});
				} else {
					c.push ({stackmachine::OP_CODE::LABEL, symbol_t ("Already allocated space for " + v->getIden () + " at offset " + std::to_string (c.stackVariables [v.get ()]))});
				}
			}
		}
		
		StackMachinePass pass (c);
		func->getBody ()->accept (pass);
		DynArray <stackmachine::TYPE> layout;
		(StackTypePass (layout)).acceptFunction (func);
		c.layouts [func->getIden ()] = layout;
		
		DynArray <std::pair <symbol_t, BasicArray <stackmachine::TYPE>>> layouts;
		for (auto & p : c.layouts) {
			layouts.push_back (p);
		}
		
		DynArray <std::pair <symbol_t, std::pair <BasicArray <symbol_t>, BasicArray <symbol_t>>>> functionLayouts;
		for (auto & p : c.functionLayouts) {
			functionLayouts.push_back (p);
		}
		
		DynArray <std::tuple <symbol_t, symbol_t, Literal>> data;
		for (auto & p : c.globals) {
			data.push_back (std::make_tuple (p.first, p.second.first, p.second.second));
		}
		
		stackmachine::Chunk chunk;
		chunk.functionLayouts = functionLayouts;
		chunk.layouts = layouts;
		chunk.data = data;
		chunk.text = c.inst;
		return chunk;
		
	} else {
		
		stackmachine::Chunk chunk;
		chunk.functionLayouts = {};
		chunk.layouts = {};
		chunk.data = {};
		return chunk;
		
	}
	
}

stackmachine::Chunk stackMachinePass (NodePtr body) {
	
	StackMachinePass::Chunk c;
	StackMachineLoadPass loadPass (c);
	body->accept (loadPass);
	StaticValue (
		{},
		body->getFile (),
		Numeric::get (Numeric::ArithmaticType::INT, 32),
		0
	).accept (loadPass);
	c.push ({stackmachine::OP_CODE::CALL_SYS, std::string ("exit")});
	
	
	DynArray <std::pair <symbol_t, BasicArray <stackmachine::TYPE>>> layouts;
	for (auto & p : c.layouts) {
		layouts.push_back (p);
	}
	
	DynArray <std::pair <symbol_t, std::pair <BasicArray <symbol_t>, BasicArray <symbol_t>>>> functionLayouts;
	for (auto & p : c.functionLayouts) {
		functionLayouts.push_back (p);
	}
	
	DynArray <std::tuple <symbol_t, symbol_t, Literal>> data;
	for (auto & p : c.globals) {
		data.push_back (std::make_tuple (p.first, p.second.first, p.second.second));
	}
	
	stackmachine::Chunk chunk;
	chunk.functionLayouts = functionLayouts;
	chunk.layouts = layouts;
	chunk.data = data;
	chunk.text = c.inst;
	return chunk;
	
}


}
