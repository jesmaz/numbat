#include <ast/memory.hpp>
#include <ast/passes/typecastpass.hpp>
#include <ast/typecast.hpp>


namespace AST {


void CastToNumberPass::visit (const Array & node) {nPtr = nullptr;}

void CastToNumberPass::visit (const ArrayInit & node) {nPtr = nullptr;}

void CastToNumberPass::visit (const Const & node) {
	node.getRegType ()->accept (*this);
}

void CastToNumberPass::visit (const Inferred & node) {
	abort ();
}

void CastToNumberPass::visit (const Interface & node) {
	abort ();
}

void CastToNumberPass::visit (const Numeric & node) {
	
	if (node.getArith () != numeric.getArith () or node.getMinPrec () != numeric.getMinPrec ()) {
		
		switch (numeric.getArith ()) {
			case Numeric::ArithmaticType::UNDETERMINED:
			case Numeric::ArithmaticType::ARBITRARY:
				nPtr = std::make_shared <CastToArbPrec> (nPtr->getPos (), nPtr->getFile (), Numeric::get (numeric.getArith (), numeric.getMinPrec ()), nPtr);
				break;
			case Numeric::ArithmaticType::DECINT:
				nPtr = std::make_shared <CastToDecimal> (nPtr->getPos (), nPtr->getFile (), Numeric::get (numeric.getArith (), numeric.getMinPrec ()), nPtr);
				break;
			case Numeric::ArithmaticType::INT:
				nPtr = std::make_shared <CastToInt> (nPtr->getPos (), nPtr->getFile (), Numeric::get (numeric.getArith (), numeric.getMinPrec ()), nPtr);
				break;
			case Numeric::ArithmaticType::FPINT:
				nPtr = std::make_shared <CastToFloat> (nPtr->getPos (), nPtr->getFile (), Numeric::get (numeric.getArith (), numeric.getMinPrec ()), nPtr);
				break;
			case Numeric::ArithmaticType::UINT:
				nPtr = std::make_shared <CastToUint> (nPtr->getPos (), nPtr->getFile (), Numeric::get (numeric.getArith (), numeric.getMinPrec ()), nPtr);
				break;
		}
		
	}
	
}

void CastToNumberPass::visit (const Ref & node) {
	auto n = std::make_shared <Load> (nPtr->getPos (), nPtr->getFile (), nPtr);
	assert (n->getType ());
	nPtr = n;
	nPtr->getType ()->accept (*this);
}

void CastToNumberPass::visit (const ReflectType & node) {nPtr = nullptr;}

void CastToNumberPass::visit (const Struct & node) {
	abort ();
}


NodePtr CastToNumberPass::operator () (const NodePtr & node) {
	assert (nPtr = node);
	assert (node->getType ());
	if (node->getType () != Numeric::get (numeric.getArith (), numeric.getMinPrec ())) {
		node->getType ()->accept (*this);
	}
	return nPtr;
}

void ImplicitCastPass::visit (const Array & node) {}

void ImplicitCastPass::visit (const ArrayInit & node) {}

void ImplicitCastPass::visit (const Const & node) {}

void ImplicitCastPass::visit (const Inferred & node) {}

void ImplicitCastPass::visit (const Interface & node) {}

void ImplicitCastPass::visit (const Numeric & node) {
	CastToNumberPass pass (node);
	nPtr = pass (nPtr);
}

void ImplicitCastPass::visit (const Ptr & node) {}

void ImplicitCastPass::visit (const Ref & node) {}

void ImplicitCastPass::visit (const ReflectType & node) {}

void ImplicitCastPass::visit (const Struct & node) {}

NodePtr ImplicitCastPass::operator () (const NodePtr & node) {
	
	auto t = node->getType ();
	if (t == target) return node;
	
	if (t->isRef ()) {
		t = t->getRegType ();
	}
	
	if (t->isConst ()) {
		
		if (target->isConst ()) {
			ImplicitCastPass imp (target->getRegType ());
			auto n = imp (node);
			score = imp.score;
			return n;
			
		} else if (target->isRef ()) {
			//Requires writable memory
			return nullptr;
			
		} else {
			
			while (t->getRegType ()) t = t->getRegType ();
			if (t == target) return node;
			
		}
		
	}
	
	nPtr = node;
	target->accept (*this);
	return nPtr;
	
}


}
