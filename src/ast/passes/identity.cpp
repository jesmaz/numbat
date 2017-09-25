#include <ast/call.hpp>
#include <ast/flowcontrol.hpp>
#include <ast/function.hpp>
#include <ast/meta.hpp>
#include <ast/node.hpp>
#include <ast/operation.hpp>
#include <ast/passes/identity.hpp>
#include <ast/sequence.hpp>
#include <ast/type.hpp>


namespace AST {


void IdentityPass::visit (const And & node) {
	nPtr = std::make_shared <And> (node.getPos (), this->visit (node.getFirst ()), this->visit (node.getSecond ()));
}

void IdentityPass::visit (const Array & node) {
}

void IdentityPass::visit (const ArrayInit & node) {
	auto len = this->visit (node.getLength ());
	auto val = this->visit (node.getVal ());
	nPtr = std::make_shared <ArrayInit> (node.getPos (), node.getType (), len, val);
}

void IdentityPass::visit (const Basic_Operation & node) {
	bool changed = false;
	
	
	auto args = node.getArgs ().map <NodePtr> ([&](auto & arg) {
		auto a = this->visit (arg);
		if (a != arg) {changed = true;}
		return a;
	});
	if (changed) {
		nPtr = std::make_shared <Basic_Operation> (node.getPos (), node.getIden (), args, node.getOpp ());
	}
}

void IdentityPass::visit (const Call_0 & node) {}

void IdentityPass::visit (const Call_1 & node) {
	auto arg = this->visit (node.getArg ());
	if (node.getArg () != arg) {
		nPtr = std::make_shared <Call_1> (node.getPos (), node.getFunc (), arg);
	}
}

void IdentityPass::visit (const Call_2 & node) {
	auto lhs = this->visit (node.getLhs ());
	auto rhs = this->visit (node.getRhs ());
	if (lhs != node.getLhs () or rhs != node.getRhs ()) {
		nPtr = std::make_shared <Call_2> (node.getPos (), node.getFunc (), lhs, rhs);
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
		nPtr = std::make_shared <Call_n> (node.getPos (), node.getFunc (), args);
	}
}

void IdentityPass::visit (const Const & node) {}

void IdentityPass::visit (const Function_Ptr & node) {}

void IdentityPass::visit (const Function_Set & node) {}

void IdentityPass::visit (const IfElse & node) {}

void IdentityPass::visit (const Inferred & node) {}

void IdentityPass::visit (const Interface & node) {}

void IdentityPass::visit (const Number & node) {}

void IdentityPass::visit (const Numeric & node) {}

void IdentityPass::visit (const Or & node) {
	nPtr = std::make_shared <Or> (node.getPos (), this->visit (node.getFirst ()), this->visit (node.getSecond ()));
}

void IdentityPass::visit (const Ref & node) {}

void IdentityPass::visit (const Reflect & node) {
	auto meta = this->visit (node.getMetaTag ());
	auto target = this->visit (node.getTarget ());
	nPtr = std::make_shared <Reflect> (node.getPos (), meta, target);
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
		nPtr = std::make_shared <Sequence> (seq.getPos (), nodes);
	}
}

void IdentityPass::visit (const Struct & node) {}

void IdentityPass::visit (const Unresolved_Call & node) {
	
	
}

void IdentityPass::visit (const Unresolved_Constructor & node) {
	
	
}

void IdentityPass::visit (const Unresolved_IfElse & node) {}

void IdentityPass::visit (const Unresolved_Operation & node) {
	
	
}

void IdentityPass::visit (const Variable & node) {}


}