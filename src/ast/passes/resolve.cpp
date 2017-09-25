#include <ast/call.hpp>
#include <ast/context.hpp>
#include <ast/flowcontrol.hpp>
#include <ast/operation.hpp>
#include <ast/passes/resolve.hpp>
#include <ast/sequence.hpp>
#include <ast/variable.hpp>
#include <iostream>


namespace AST {


void ResolvePass::visit (const Sequence & node) {
	auto nodes = node.getNodes ().map <NodePtr> ([&](auto & n) {
		return this->visit (n);
	});
	nPtr = std::make_shared <Sequence> (node.getPos (), nodes.back ()->getType (), nodes);
}

void ResolvePass::visit (const Variable & node) {
	auto type = ResolveTypePass () (node.getType ());
	nPtr = std::make_shared <Variable> (node.getPos (), node.getIden (), type);
}

void ResolvePass::visit (const Unresolved_Call & node) {
	auto args = node.getArgs ().map <NodePtr> ([&](auto & arg) {
		return this->visit (arg);
	});
	nPtr = MakeCallPass (node.getPos (), args) (node.getCallee ());
	assert (nPtr);
}

void ResolvePass::visit (const Unresolved_Constructor & node) {
	auto args = node.getArgs ().map <NodePtr> ([&](auto & arg) {
		return this->visit (arg);
	});
	auto var = this->visit (node.getVar ());
	nPtr = ConstructorSelectionPass (node.getPos (), node.getVar (), args) (node.getVar ()->getType ());
}

void ResolvePass::visit (const Unresolved_Operation & node) {
	auto args = node.getArgs ().map <NodePtr> ([&](auto & arg) {
		return this->visit (arg);
	});
	
	auto types = args.map <TypePtr> ([](auto & arg) {
		auto type = arg->getType ();
		while (type->getRegType ()) {
			type = type->getRegType ();
		}
		return type;
	});
	
	if (/*TODO: Check for complex types*/ false) {
		
	} else if (args.size () == 1 or types [0] == types [1]) {
		nPtr = std::make_shared <Basic_Operation> (node.getPos (), types [0], node.getIden (), args, node.getOpp ());
		
	} else if (args.size () == 2) {
		auto arbNum = Numeric::get (Numeric::ArithmaticType::ARBITRARY, 0);
		if (types [0] == arbNum and typeid (*types [1]) == typeid (Numeric)) {
			nPtr = std::make_shared <Basic_Operation> (node.getPos (), types [1], node.getIden (), args, node.getOpp ());
		} else if (types [1] == arbNum and typeid (*types [0]) == typeid (Numeric)) {
			nPtr = std::make_shared <Basic_Operation> (node.getPos (), types [0], node.getIden (), args, node.getOpp ());
		} else {
			//TODO: raise an error
			abort ();
		}
		
	} else {
		nPtr = std::make_shared <Basic_Operation> (node.getPos (), args [0]->getType (), node.getIden (), args, node.getOpp ());
		
	}
	
}

void ResolvePass::visit (const Unresolved_IfElse & node) {
	auto cond = this->visit (node.getCond ());
	auto body = this->visit (node.getBody ());
	auto alt = node.getAlt () ? this->visit (node.getAlt ()) : nullptr;
	
	//TODO: Make sure body & alt types are compatible with the return type
	
	VarPtr var = std::make_shared <Variable> (node.getPos (), "tmp", body->getType ());
	body = std::make_shared <Basic_Operation> (node.getPos (), " = ", BasicArray <NodePtr> {var, body}, parser::OPERATION::ASSIGN);
	if (alt) {
		alt = std::make_shared <Basic_Operation> (node.getPos (), " = ", BasicArray <NodePtr> {var, alt}, parser::OPERATION::ASSIGN);
	}
	
	nPtr = std::make_shared <IfElse> (node.getPos (), var, cond, body, alt);
}

void ResolveTypePass::visit (const ReflectType & node) {
	//TODO: resolve meta tag to specific function
}

TypePtr ResolveTypePass::operator () (const TypePtr & node) {
	assert (nType = node);
	node->accept (*this);
	assert (nType);
	return nType;
}

void MakeCallPass::visit (const Function_Ptr & node) {
	
}

void MakeCallPass::visit (const Function_Set & node) {
	
	FuncPtr f;
	BasicArray <NodePtr> params = args;
	for (auto & func : node.getSet ().funcs) {
		bool accept = true;
		BasicArray <NodePtr> tmpParams = func.first->getParams ().map <NodePtr> ([&](const NodePtr & ptr, size_t i) {
			if (i < args.size ()) {
				//TODO: check for type compatibility
				return args [i];
			} else {
				accept = false;
			}
			return ptr;
		});
		if (accept) {
			f = func.first;
			params = tmpParams;
		}
	}
	
	if (!f) {
		//TODO: print error
	} else {
		
		switch (params.size ()) {
			case 0:
				nPtr = std::make_shared <Call_0> (pos, f);
				break;
			case 1:
				nPtr = std::make_shared <Call_1> (pos, f, params [0]);
				break;
			case 2:
				nPtr = std::make_shared <Call_2> (pos, f, params [0], params [1]);
				break;
			default:
				nPtr = std::make_shared <Call_n> (pos, f, params);
				break;
		}
	}
}

void MakeCallPass::visit (const Struct & node) {
	
}

void MakeCallPass::visit (const Variable & node) {
	
}

NodePtr MakeCallPass::operator () (const NodePtr & node) {
	nPtr = node;
	node->accept (*this);
	return nPtr;
}


void CallMethodPass::visit (const Array & node) {
}

void CallMethodPass::visit (const Const & node) {
}

void CallMethodPass::visit (const Interface & node) {
}

void CallMethodPass::visit (const Numeric & node) {
}

void CallMethodPass::visit (const Ref & node) {
}

void CallMethodPass::visit (const Struct & node) {
}


void ConstructorSelectionPass::visit (const Array & node) {
	
}

void ConstructorSelectionPass::visit (const ArrayInit & node) {
	replacementType = node.getType ();
	//auto init = std::make_shared <
	//nPtr = std::make_shared <Loop> ();
}

void ConstructorSelectionPass::visit (const Const & node) {
	node.getRegType ()->accept (*this);
	if (replacementType) {
		//TODO: Make replacement type const
	}
}

void ConstructorSelectionPass::visit (const Inferred & node) {
	if (args.empty ()) {
		//TODO: Set to empty tuple
		abort ();
	} if (args.size () == 1) {
		std::cerr << args.front ()->toString (text::PrintMode::COLOUR) << std::endl;
		assert (replacementType = args.front ()->getType ());
		nPtr = std::make_shared <Basic_Operation> (node.getPos (), " = ", BasicArray <NodePtr> {var, args.front ()}, parser::OPERATION::ASSIGN);
	} else {
		//TODO: Set to tuple
		abort ();
	}
}

void ConstructorSelectionPass::visit (const Interface & node) {
	//TODO: check to see if the interface matches  the RHS
	abort ();
}

void ConstructorSelectionPass::visit (const Numeric & node) {
	//TODO: Check arguments for compatibility
}

void ConstructorSelectionPass::visit (const Ref & node) {
	node.getRegType ()->accept (*this);
	if (replacementType) {
		//TODO: Make replacement type a reference
	}
}

void ConstructorSelectionPass::visit (const ReflectType & node) {
	//TODO: Search type for constructors
}

void ConstructorSelectionPass::visit (const Struct & node) {
	//TODO: Search struct for constructors
	abort ();
}

NodePtr ConstructorSelectionPass::operator () (const NodePtr & node) {
	nPtr = var;
	node->accept (*this);
	if (replacementType) {
		*var = Variable (var->getPos (), var->getIden (), replacementType);
	}
	return nPtr;
}


};
