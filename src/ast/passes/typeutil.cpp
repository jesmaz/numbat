#include <ast/meta.hpp>
#include <ast/passes/typeutil.hpp>
#include <ast/type.hpp>


namespace AST {


void DominantType::visit (const Array & node) {}

void DominantType::visit (const Numeric & node) {
	
	if (typeid (*b) == typeid (Numeric)) {
		
		auto & bType = static_cast <const Numeric &> (*b);
		
		switch (node.getArith ()) {
			case Numeric::ArithmaticType::UNDETERMINED:
				a = b;
				break;
			case Numeric::ArithmaticType::UINT:
				if (bType.getArith () == Numeric::ArithmaticType::INT) {
					a = b;
				}
			case Numeric::ArithmaticType::INT:
				if (bType.getArith () == Numeric::ArithmaticType::FPINT) {
					a = b;
				}
			case Numeric::ArithmaticType::FPINT:
				if (bType.getArith () == Numeric::ArithmaticType::DECINT) {
					a = b;
				}
			case Numeric::ArithmaticType::DECINT:
				if (bType.getArith () == Numeric::ArithmaticType::ARBITRARY) {
					a = b;
				}
			case Numeric::ArithmaticType::ARBITRARY:
				break;
		}
		
	} else {
		a = b;
	}
	
}

void DominantType::visit (const ReflectType & node) {}

void DominantType::visit (const Struct & node) {}


TypePtr DominantType::operator () () {
	while (a->getRegType ()) a = a->getRegType ();
	while (b->getRegType ()) b = b->getRegType ();
	a->accept (*this);
	return a;
	
}


}