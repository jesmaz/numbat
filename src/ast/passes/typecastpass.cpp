#include <ast/passes/typecastpass.hpp>
#include <ast/typecast.hpp>


namespace AST {


void CastToNumberPass::visit (const Array & node) {
	abort ();
}

void CastToNumberPass::visit (const ArrayInit & node) {
	abort ();
}

void CastToNumberPass::visit (const Const & node) {
	abort ();
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
	abort ();
}

void CastToNumberPass::visit (const ReflectType & node) {
	abort ();
}

void CastToNumberPass::visit (const Struct & node) {
	abort ();
}


NodePtr CastToNumberPass::operator () (const NodePtr & node) {
	assert (nPtr = node);
	assert (node->getType ());
	node->getType ()->accept (*this);
	assert (nPtr);
	return nPtr;
}

void ImplicitCastPass::visit (const Array & node) {}

void ImplicitCastPass::visit (const ArrayInit & node) {}

void ImplicitCastPass::visit (const Const & node) {}

void ImplicitCastPass::visit (const Inferred & node) {}

void ImplicitCastPass::visit (const Interface & node) {}

void ImplicitCastPass::visit (const Numeric & node) {}

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
	t->accept (*this);
	return nPtr;
	
}


NodePtr StaticCastPass::operator () (const NodePtr & node) {
	
	if (node->getType () == target) return node;
	abort ();
	
}


}
