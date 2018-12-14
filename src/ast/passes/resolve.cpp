#include <ast/call.hpp>
#include <ast/context.hpp>
#include <ast/flowcontrol.hpp>
#include <ast/operation.hpp>
#include <ast/passes/funcutil.hpp>
#include <ast/passes/resolve.hpp>
#include <ast/passes/typecastpass.hpp>
#include <ast/passes/typeutil.hpp>
#include <ast/sequence.hpp>
#include <ast/type.hpp>
#include <ast/variable.hpp>
#include <iostream>
#include <utility/report.hpp>


namespace AST {


void ResolveMemberPass::visit (const Array & node) {
	if (iden == "data" or iden == "d") {
		index = 0;
		type = Const::get (Ref::get (node.getBaseType ()));
	} else if (iden == "length" or iden == "len" or iden == "l") {
		index = 1;
		type = Const::get (Numeric::get (Numeric::ArithmaticType::UINT, 0));
	} else {
		index = -1;
	}
}

void ResolveMemberPass::visit (const Const & node) {
	abort ();
}

void ResolveMemberPass::visit (const Import & node) {
	nPtr = node.getFile ()->getContext ().resolveType (iden);
	if (not nPtr) {
		nPtr = node.getFile ()->getContext ().resolve (iden);
	}
}

void ResolveMemberPass::visit (const Interface & node) {
	abort ();
}

void ResolveMemberPass::visit (const Numeric & node) {
	abort ();
}

void ResolveMemberPass::visit (const Ref & node) {
	abort ();
}

void ResolveMemberPass::visit (const Struct & node) {
	auto itt = node.getPositionMap ().find (iden);
	if (itt != node.getPositionMap ().end ()) {
		index = itt->second;
		type = node.getMembers () [index]->getType ();
	}
}

NodePtr ResolveMemberPass::operator ()(numbat::lexer::position pos, const numbat::File * file, const NodePtr & node) {
	node->getType ()->accept (*this);
	if (nPtr) return nPtr;
	if (index >= 0) {
		return std::make_shared <StaticIndex> (pos, file, type, node, index);
	} else {
		report::logMessage (report::Severity::ERROR, file, pos, "'" + iden + "' is not a member of " + node->toString (text::PrintMode::PLAIN));
		return node;
	}
}


void ResolvePass::visit (const Sequence & node) {
	auto nodes = node.getNodes ().map <NodePtr> ([&](auto & n) {
		return this->visit (n);
	});
	if (nodes.empty ()) {
		nPtr = std::make_shared <Sequence> (
			node.getPos (),
			node.getFile (),
			std::make_shared <Struct> (node.getPos (), node.getFile (), ""),
			node.getLocalStack (),
			nodes
		);
	} else {
		nPtr = std::make_shared <Sequence> (node.getPos (), node.getFile (), nodes.back ()->getType (), node.getLocalStack (), nodes);
	}
}

void ResolvePass::visit (const Variable & node) {
	auto type = ResolveTypePass () (node.getType ());
	*std::dynamic_pointer_cast <Variable> (nPtr) = Variable (node.getPos (), node.getFile (), type, node.getStackIndex (), node.getLocation (), node.getIden ());
}

void ResolvePass::visit (const Unresolved_Call & node) {
	auto args = node.getArgs ().map <NodePtr> ([&](auto & arg) {
		return this->visit (arg);
	});
	auto callee = ResolvePass () (node.getCallee ());
	nPtr = MakeCallPass (node.getPos (), node.getFile (), args) (callee);
	assert (nPtr);
}

void ResolvePass::visit (const Unresolved_Constructor & node) {
	auto args = node.getArgs ().map <NodePtr> ([&](auto & arg) {
		return this->visit (arg);
	});
	auto var = this->visit (node.getVar ());
	nPtr = ConstructorSelectionPass (node.getPos (), node.getFile (), node.getVar (), args) (node.getVar ()->getType ());
}

void ResolvePass::visit (const Unresolved_Get_Member & node) {
	
	auto parent = this->visit (node.getParent ());
	nPtr = ResolveMemberPass (node.getmember ()) (node.getPos (), node.getFile (), parent);
	
}

void ResolvePass::visit (const Unresolved_IfElse & node) {
	auto cond = this->visit (node.getCond ());
	auto body = this->visit (node.getBody ());
	auto alt = node.getAlt () ? this->visit (node.getAlt ()) : nullptr;
	
	//TODO: Make sure body & alt types are compatible with the return type
	
	VarPtr var = std::make_shared <Variable> (node.getPos (), node.getFile (), body->getType (), Variable::globalContex.reserve (), Variable::LOCATION::GLOBAL, "tmp");
	body = std::make_shared <Basic_Operation> (node.getPos (), node.getFile (), " = ", BasicArray <NodePtr> {var, body}, parser::OPERATION::ASSIGN);
	if (alt) {
		alt = std::make_shared <Basic_Operation> (node.getPos (), node.getFile (), " = ", BasicArray <NodePtr> {var, alt}, parser::OPERATION::ASSIGN);
	}
	
	nPtr = std::make_shared <IfElse> (node.getPos (), node.getFile (), var, cond, body, alt);
}

void ResolvePass::visit (const Unresolved_Loop & node) {
	NodePtr i, c, s, b;
	if (node.getInit ()) {
		i = this->visit (node.getInit ());
	}
	c = this->visit (node.getCond ());
	if (node.getStep ()) {
		s = this->visit (node.getStep ());
	}
	b = this->visit (node.getBody ());
	
	nPtr = std::make_shared <Loop> (node.getPos (), node.getFile (), i, c, s, b, nullptr);
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
		
	} else if (isPredicate (node.getOpp ())) {
		if (args.size () == 2) {
			if (types [0] == types [1]) {
				nPtr = std::make_shared <Basic_Operation> (
					node.getPos (),
					node.getFile (),
					Numeric::get (Numeric::ArithmaticType::UINT, 1),
					node.getIden (),
					BasicArray <NodePtr> ({
						args [0],
						args [1]
					}),
					node.getOpp ()
				);
			} else if (typeid (*types [0]) == typeid (Array) and typeid (*types [1]) == typeid (Array)) {
				
				if (node.getOpp () == parser::OPERATION::CMPEQ) {
					/*auto boolType = Numeric::get (Numeric::ArithmaticType::UINT, 1);
					auto condArgs = BasicArray <NodePtr> ({
						ResolveMemberPass ("len") (node.getPos (), node.getFile (), args [0]), 
						ResolveMemberPass ("len") (node.getPos (), node.getFile (), args [1])
					});
					auto cond = std::make_shared <Basic_Operation> (node.getPos (), node.getFile (), boolType, node.getIden (), condArgs);
					auto loopInit = std::make_shared <Variable> (node.getPos (), node.getFile ());
					auto compareLoop = std::make_shared <Loop> (node.getPos (), node.getFile ());
					nPtr = std::make_shared <IfElse> (node.getPos (), node.getFile (), cond);*/
					
				} else {
					abort ();
				}
				
			} else {
				auto dom = DominantType (types [0], types [1]) ();
				nPtr = std::make_shared <Basic_Operation> (
					node.getPos (),
					node.getFile (),
					Numeric::get (Numeric::ArithmaticType::UINT, 1),
					node.getIden (),
					BasicArray <NodePtr> ({
						StaticCastPass (dom) (args [0]),
						StaticCastPass (dom) (args [1])
					}),
					node.getOpp ()
				);
				
			}
		} else {
			nPtr = std::make_shared <Basic_Operation> (node.getPos (), node.getFile (), Numeric::get (Numeric::ArithmaticType::UINT, 1), node.getIden (), args, node.getOpp ());
		}
		
	} else if (args.size () == 1 or types [0] == types [1]) {
		nPtr = std::make_shared <Basic_Operation> (node.getPos (), node.getFile (), types [0], node.getIden (), args, node.getOpp ());
		
	} else if (args.size () == 2) {
		if (typeid (*types [0]) == typeid (Numeric) and typeid (*types [1]) == typeid (Numeric)) {
			auto dom = DominantType (types [0], types [1]) ();
			nPtr = std::make_shared <Basic_Operation> (
				node.getPos (),
				node.getFile (),
				types [1],
				node.getIden (),
				BasicArray <NodePtr> ({
					StaticCastPass (dom) (args [0]),
					StaticCastPass (dom) (args [1])
				}),
				node.getOpp ()
			);
			
		} else {
			//TODO: raise an error
			abort ();
		}
		
	} else {
		nPtr = std::make_shared <Basic_Operation> (node.getPos (), node.getFile (), args [0]->getType (), node.getIden (), args, node.getOpp ());
		
	}
	
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
	
	size_t winningScore = __UINT64_MAX__;
	NodePtr winner;
	for (auto & f : node.getSet ().funcs) {
		size_t score;
		auto c = createCall (pos, file, f.first, args, score);
		if (c) {
			if (score < winningScore) {
				winner = c;
			} else if (score == winningScore) {
				winner = nullptr;
			}
		}
	}
	
	if (winner) {
		nPtr = winner;
	} else {
		abort ();
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
		nPtr = std::make_shared <Basic_Operation> (node.getPos (), node.getFile (), " = ", BasicArray <NodePtr> {var, args.front ()}, parser::OPERATION::ASSIGN);
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
	if (args.size () == 1) {
		auto arg = CastToNumberPass (node) (args [0]);
		nPtr = std::make_shared <Basic_Operation> (var->getPos (), var->getFile (), " = ", BasicArray <NodePtr> {var, arg}, parser::OPERATION::ASSIGN);
	}
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
	auto itt = node.getMethods ().lower_bound ("");
	auto end = node.getMethods ().upper_bound ("");
	size_t winningScore = __UINT64_MAX__;
	NodePtr winner;
	while (itt != end) {
		size_t score;
		auto c = createCall (var->getPos (), var->getFile (), itt->second, args, score);
		if (c) {
			if (score < winningScore) {
				winner = c;
			} else if (score == winningScore) {
				winner = nullptr;
			}
		}
		++itt;
	}
	
	if (winner) {
		nPtr = std::make_shared <Basic_Operation> (var->getPos (), var->getFile (), var->getType (), " = ", BasicArray <NodePtr> {var, winner}, parser::OPERATION::ASSIGN);
	} else {
		abort ();
	}
}

NodePtr ConstructorSelectionPass::operator () (const NodePtr & node) {
	nPtr = var;
	node->accept (*this);
	if (replacementType) {
		*var = Variable (var->getPos (), var->getFile (), replacementType, var->getStackIndex (), var->getLocation (), var->getIden ());
	}
	return nPtr;
}


};
