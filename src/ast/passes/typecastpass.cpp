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
			case Numeric::ArithmaticType::ARBITRARY:
				nPtr = std::make_shared <CastToArbPrec> (nPtr->getPos (), Numeric::get (numeric.getArith (), numeric.getMinPrec ()), nPtr);
				break;
			case Numeric::ArithmaticType::DECINT:
				nPtr = std::make_shared <CastToDecimal> (nPtr->getPos (), Numeric::get (numeric.getArith (), numeric.getMinPrec ()), nPtr);
				break;
			case Numeric::ArithmaticType::INT:
				nPtr = std::make_shared <CastToInt> (nPtr->getPos (), Numeric::get (numeric.getArith (), numeric.getMinPrec ()), nPtr);
				break;
			case Numeric::ArithmaticType::FPINT:
				nPtr = std::make_shared <CastToFloat> (nPtr->getPos (), Numeric::get (numeric.getArith (), numeric.getMinPrec ()), nPtr);
				break;
			case Numeric::ArithmaticType::UINT:
				nPtr = std::make_shared <CastToUint> (nPtr->getPos (), Numeric::get (numeric.getArith (), numeric.getMinPrec ()), nPtr);
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


}
