#include <ast/call.hpp>
#include <ast/flowcontrol.hpp>
#include <ast/function.hpp>
#include <ast/memory.hpp>
#include <ast/meta.hpp>
#include <ast/node.hpp>
#include <ast/operation.hpp>
#include <ast/passes/identity.hpp>
#include <ast/sequence.hpp>
#include <ast/type.hpp>
#include <ast/variable.hpp>


namespace AST {


void IdentityPass::visit (const And & node) {
	nPtr = std::make_shared <And> (node.getPos (), node.getFile (), this->visit (node.getFirst ()), this->visit (node.getSecond ()));
}

void IdentityPass::visit (const Array & node) {
}

void IdentityPass::visit (const ArrayInit & node) {
	auto len = this->visit (node.getLength ());
	if (node.getVal ()) {
		auto val = this->visit (node.getVal ());
		nPtr = std::make_shared <ArrayInit> (node.getPos (), node.getFile (), node.getType (), len, val);
	} else {
		nPtr = std::make_shared <ArrayInit> (node.getPos (), node.getFile (), node.getType (), len, nullptr);
	}
	
}

void IdentityPass::visit (const Basic_Operation & node) {
	bool changed = false;
	
	
	auto args = node.getArgs ().map <NodePtr> ([&](auto & arg) {
		auto a = this->visit (arg);
		if (a != arg) {changed = true;}
		return a;
	});
	if (changed) {
		nPtr = std::make_shared <Basic_Operation> (node.getPos (), node.getFile (), node.getIden (), args, node.getOpp ());
	}
}

void IdentityPass::visit (const Call_0 & node) {}

void IdentityPass::visit (const Call_1 & node) {
	auto arg = this->visit (node.getArg ());
	if (node.getArg () != arg) {
		nPtr = std::make_shared <Call_1> (node.getPos (), node.getFile (), node.getType (), node.getFunc (), arg);
	}
}

void IdentityPass::visit (const Call_2 & node) {
	auto lhs = this->visit (node.getLhs ());
	auto rhs = this->visit (node.getRhs ());
	if (lhs != node.getLhs () or rhs != node.getRhs ()) {
		nPtr = std::make_shared <Call_2> (node.getPos (), node.getFile (), node.getType (), node.getFunc (), lhs, rhs);
	}
}

void IdentityPass::visit (const Call_n & node) {
	bool changed = false;
	
	
	auto args = node.getArgs ().map <NodePtr> ([&](auto & arg) {
		auto a = this->visit (arg);
		if (a != arg) {changed = true;}
		return a;
	});
	if (changed) {
		nPtr = std::make_shared <Call_n> (node.getPos (), node.getFile (), node.getType (), node.getFunc (), args);
	}
}

void IdentityPass::visit (const CastToArbPrec & node) {}

void IdentityPass::visit (const CastToDecimal & node) {}

void IdentityPass::visit (const CastToFloat & node) {}

void IdentityPass::visit (const CastToInt & node) {}

void IdentityPass::visit (const CastToUint & node) {}

void IdentityPass::visit (const Const & node) {}

void IdentityPass::visit (const Function_Ptr & node) {}

void IdentityPass::visit (const Function_Set & node) {}

void IdentityPass::visit (const IfElse & node) {}

void IdentityPass::visit (const Inferred & node) {}

void IdentityPass::visit (const Interface & node) {}

void IdentityPass::visit (const Load & node) {
	auto c = this->visit (node.getChild ());
	if (c != node.getChild ()) {
		nPtr = std::make_shared <Load> (node.getPos (), node.getFile (), c);
	}
}

void IdentityPass::visit (const Loop & node) {
	NodePtr i, c, s, b;
	if (node.getInit ()) {
		i = this->visit (node.getInit ());
	}
	c = this->visit (node.getCond ());
	if (node.getStep ()) {
		s = this->visit (node.getStep ());
	}
	b = this->visit (node.getBody ());
	
	nPtr = std::make_shared <Loop> (
		node.getPos (),
		node.getFile (),
		i, c, s, b,
		node.getArray ()
	);
}

void IdentityPass::visit (const Numeric & node) {}

void IdentityPass::visit (const Or & node) {
	nPtr = std::make_shared <Or> (node.getPos (), node.getFile (), this->visit (node.getFirst ()), this->visit (node.getSecond ()));
}

void IdentityPass::visit (const Ptr & node) {}

void IdentityPass::visit (const RawInit & node) {
	nPtr = std::make_shared <RawInit> (
		node.getPos (),
		node.getFile (),
		node.getVar (),
		node.getArgs ().map <NodePtr> ([&](auto & n) {
			return this->visit (n);
		})
	);
}

void IdentityPass::visit (const Ref & node) {}

void IdentityPass::visit (const Reflect & node) {
	auto meta = this->visit (node.getMetaTag ());
	auto target = this->visit (node.getTarget ());
	nPtr = std::make_shared <Reflect> (node.getPos (), node.getFile (), meta, target);
}

void IdentityPass::visit (const ReflectType & node) {}

void IdentityPass::visit (const Sequence & seq) {
	bool changed = false;
	
	auto nodes = seq.getNodes ().map <NodePtr> ([&](auto & n) {
		auto a = this->visit (n);
		assert (a);
		if (a != n) {changed = true;}
		return a;
	});
	if (changed) {
		nPtr = std::make_shared <Sequence> (seq.getPos (), seq.getFile (), seq.getLocalStack (), nodes);
	}
}

void IdentityPass::visit (const StaticIndex & node) {
	nPtr = std::make_shared <StaticIndex> (node.getPos (), node.getFile (), node.getType (), this->visit (node.getParent ()), node.getIndex ());
}

void IdentityPass::visit (const Struct & node) {}

void IdentityPass::visit (const SystemCall & node) {
	bool changed = false;
	
	
	auto args = node.getArgs ().map <NodePtr> ([&](auto & arg) {
		auto a = this->visit (arg);
		if (a != arg) {changed = true;}
		return a;
	});
	if (changed) {
		nPtr = std::make_shared <SystemCall> (node.getPos (), node.getFile (), node.getType (), node.getSysName (), args);
	}
}

void IdentityPass::visit (const Unresolved_Call & node) {
	
	
}

void IdentityPass::visit (const Unresolved_Constructor & node) {
	
	
}

void IdentityPass::visit (const Unresolved_Get_Member & node) {}

void IdentityPass::visit (const Unresolved_IfElse & node) {}

void IdentityPass::visit (const Unresolved_Loop & node) {}

void IdentityPass::visit (const Unresolved_Operation & node) {
	
	
}

void IdentityPass::visit (const Variable & node) {}

void IdentityPass::visit (const VariableRef & node) {}


}
