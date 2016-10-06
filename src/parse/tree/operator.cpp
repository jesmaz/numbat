#include <nir/scope.hpp>
#include <parse/tree/operator.hpp>
#include <utility/report.hpp>


namespace parser {


BasicArray <nir::Argument> resolveNodes (nir::Scope * scope, BasicArray <PTNode> args) {
	
	BasicArray <nir::Argument> nodes (args.size ());
	for (size_t i=0; i<args.size(); ++i) {
		assert (args[i]);
		const nir::Instruction * instr = args[i]->build (scope);
		if (instr and not instr->getResults ().empty ()) {
			nodes [i] = {instr, instr->getIden ()};
		} else {
			return {};
		}
	}
	
	return nodes;
	
}


const nir::Instruction * GenericOperator::build (nir::Scope * scope) {
	return defBuild (scope);
}


string GenericOperator::strDump (text::PrintMode mode) {
	if (iden.empty () and not args.empty ()) {
		string s = "(";
		for (size_t i=0; i+1<args.size (); ++i) s += args [i]->toString (mode) + " ";
		return s + args.back ()->toString (mode) + ")";
	} else if (not args.empty ()) {
		string s = "(";
		size_t i=0;
		for (char c : iden) {
			if (c == ' ') {
				if (isalnum (s.back ())) s += " ";
				assert (i < args.size ());
				s += args [i]->toString (mode) + " ";
				++i;
			} else {
				s.push_back (c);
				if (!isalnum (c)) s += " ";
			}
		}
		s.back () = ')';
		return s;
	} else {
		return "()";
	}
}


template <>
const nir::Instruction * SpecificOperator <OPERATION::ADD>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createAdd (nodes [0], nodes [1]);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::AND>::defBuild (nir::Scope * scope) {
	
	//TODO: Return a union
	const nir::Type * b = scope->resolveType ("bool");
	const nir::Instruction * res = scope->allocateVariable (b);
	const nir::Instruction * lhs = args [0]->build (scope);
	if (not lhs or lhs->getResults ().empty ()) return nullptr;
	
	nir::symbol rhsBlock = scope->createBlock ();
	nir::symbol cont = scope->createBlock ();
	
	scope->createPut ({lhs, lhs->getIden ()}, {res, res->getIden ()});
	const nir::Instruction * cond = scope->createBitNot ({lhs, lhs->getIden ()});
	scope->createJump ({cond, cond->getIden ()}, cont);
	scope->changeActiveBlock (rhsBlock);
	
	const nir::Instruction * rhs = args [1]->build (scope);
	if (not rhs or rhs->getResults ().empty ()) return nullptr;
	
	scope->createPut ({rhs, rhs->getIden ()}, {res, res->getIden ()});
	scope->changeActiveBlock (cont);
	
	return scope->createGet ({res, res->getIden ()});
	
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::AS>::defBuild (nir::Scope * scope) {
	const nir::Instruction * lhs = args [0]->build (scope);
	const nir::Type * type = args [1]->resolveType (scope);
	if (not lhs or not type) return nullptr;
	const nir::Type * ltype = lhs->getType ()->getDereferenceType ();
	if (not ltype) {
		report::logMessage (report::ERROR, scope->getSourceFile (), getPos (), "'" + lhs->toString (text::PLAIN) + "' can't be reinterpreted");
		return nullptr;
	}
	//TODO: Ensure that lhs type is trivial (nothing that could create invalid state like a pointer)
	if (ltype->calculateSize (sizeof (size_t)) < type->calculateSize (sizeof (size_t))) {
		report::logMessage (report::ERROR, scope->getSourceFile (), getPos (), "'" + lhs->toString (text::PLAIN) + "' is too small, reinterpeting would lead to courupted memory");
		return nullptr;
	}
	type = type->getPointerTo ();
	return scope->createReinterpret ({lhs, lhs->getIden ()}, type);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::ASSIGN>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createAssign (nodes [0], nodes [1]);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::BAND>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createBitAnd (nodes [0], nodes [1]);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::BNOT>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createBitNot (nodes [0]);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::BOR>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createBitOr (nodes [0], nodes [1]);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::BXOR>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createBitXor (nodes [0], nodes [1]);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::CMPEQ>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createCmpEQ (nodes [0], nodes [1]);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::CMPGT>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createCmpGT (nodes [0], nodes [1]);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::CMPGTE>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createCmpGTE (nodes [0], nodes [1]);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::CMPLT>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createCmpLT (nodes [0], nodes [1]);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::CMPLTE>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createCmpLTE (nodes [0], nodes [1]);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::CMPNE>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createCmpNE (nodes [0], nodes [1]);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::CONCAT>::defBuild (nir::Scope * scope) {
	report::logMessage (report::ERROR, scope->getSourceFile (), getPos (), "Can't concatenate '" + args [0]->toString (text::PLAIN) + "' to '" + args [0]->toString (text::PLAIN) + "'");
	return nullptr;
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::DECREMENT>::defBuild (nir::Scope * scope) {
	abort ();
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::DIV>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createDiv (nodes [0], nodes [1]);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::IN>::defBuild (nir::Scope * scope) {
	report::logMessage (report::ERROR, scope->getSourceFile (), getPos (), "'" + args [0]->toString (text::PLAIN) + "' does not support the in operation");
	return nullptr;
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::INCREMENT>::defBuild (nir::Scope * scope) {
	abort ();
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::LNOT>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createLNot (nodes [0]);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::MUL>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createMul (nodes [0], nodes [1]);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::NEG>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createNeg (nodes [0]);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::NONE>::defBuild (nir::Scope * scope) {
	report::logMessage (report::ERROR, scope->getSourceFile (), getPos (), "Operator '" + iden + "' is not currently supported");
	return nullptr;
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::OR>::defBuild (nir::Scope * scope) {
	
	//TODO: Return a union
	const nir::Type * b = scope->resolveType ("bool");
	const nir::Instruction * res = scope->allocateVariable (b);
	const nir::Instruction * lhs = args [0]->build (scope);
	if (not lhs or lhs->getResults ().empty ()) return nullptr;
	
	nir::symbol rhsBlock = scope->createBlock ();
	nir::symbol cont = scope->createBlock ();
	
	scope->createPut ({lhs, lhs->getIden ()}, {res, res->getIden ()});
	scope->createJump ({lhs, lhs->getIden ()}, cont);
	scope->changeActiveBlock (rhsBlock);
	
	const nir::Instruction * rhs = args [1]->build (scope);
	if (not rhs or rhs->getResults ().empty ()) return nullptr;
	
	scope->createPut ({rhs, rhs->getIden ()}, {res, res->getIden ()});
	scope->changeActiveBlock (cont);
	
	return scope->createGet ({res, res->getIden ()});
	
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::REM>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createRem (nodes [0], nodes [1]);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::SUB>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createSub (nodes [0], nodes [1]);
}


}
