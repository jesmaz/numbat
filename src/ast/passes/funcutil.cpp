#include <ast/call.hpp>
#include <ast/function.hpp>
#include <ast/passes/funcutil.hpp>
#include <ast/passes/typecastpass.hpp>


namespace AST {


NodePtr createCall (numbat::lexer::position pos, const numbat::File * file, const FuncPtr & func, const BasicArray <NodePtr> & args, size_t & score) {
	
	//TODO: Take into consideration parameter defaults
	
	if (func->getParams ().size () != args.size ()) {
		return nullptr;
	}
	score = 0;
	
	BasicArray <NodePtr> processed (args.size ());
	for (size_t i=0, l=args.size (); i<l; ++i) {
		ImplicitCastPass imp (func->getParams () [i]);
		processed [i] = imp (args [i]);
		if (not processed [i]) {
			return nullptr;
		}
		score += imp.getScore ();
	}
	
	TypePtr type;
	
	if (func->getRetVals ().size () == 1) {
		type = func->getRetVals ().front ();
	} else {
		type = Struct::tuple (func->getRetVals ());
	}
	
	switch (processed.size ()) {
		case 0:
			return std::make_shared <Call_0> (pos, file, type, func);
			break;
		case 1:
			return std::make_shared <Call_1> (pos, file, type, func, processed [0]);
			break;
		case 2:
			return std::make_shared <Call_2> (pos, file, type, func, processed [0], processed [1]);
			break;
		default:
			return std::make_shared <Call_n> (pos, file, type, func, processed);
			break;
	}
	
}


}
