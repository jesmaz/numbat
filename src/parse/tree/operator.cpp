#include <nir/scope.hpp>
#include <parse/tree/operator.hpp>
#include <utility/report.hpp>


std::vector <nir::Argument> resolveNodes (nir::Scope * scope, std::vector <PTNode> args) {
	
	std::vector <nir::Argument> nodes;
	nodes.resize (args.size (), {nullptr, nullptr});
	//std::vector <numbat::parser::FunctionDecleration *> candidates;
	for (size_t i=0; i<args.size(); ++i) {
		assert (args[i]);
		const nir::Instruction * instr = args[i]->build (scope);
		if (instr) {
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
	return scope->createAdd (nodes);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::AND>::defBuild (nir::Scope * scope) {
	abort ();
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::AS>::defBuild (nir::Scope * scope) {
	abort ();
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::ASSIGN>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createAssign (nodes);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::BAND>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createBitAnd (nodes);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::BNOT>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createBitNot (nodes);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::BOR>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createBitOr (nodes);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::BXOR>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createBitXor (nodes);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::CMPEQ>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createCmpEQ (nodes);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::CMPGT>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createCmpGT (nodes);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::CMPGTE>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createCmpGTE (nodes);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::CMPLT>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createCmpLT (nodes);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::CMPLTE>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createCmpLTE (nodes);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::CMPNE>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createCmpNE (nodes);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::CONCAT>::defBuild (nir::Scope * scope) {
	abort ();
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::DECREMENT>::defBuild (nir::Scope * scope) {
	abort ();
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::DIV>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createDiv (nodes);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::IN>::defBuild (nir::Scope * scope) {
	abort ();
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::INCREMENT>::defBuild (nir::Scope * scope) {
	abort ();
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::LNOT>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createLNot (nodes);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::MUL>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createMul (nodes);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::NEG>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createNeg (nodes);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::NONE>::defBuild (nir::Scope * scope) {
	report::logMessage (report::ERROR, "", getLine (), getPos (), "Operator '" + iden + "' is not currently supported");
	return nullptr;
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::OR>::defBuild (nir::Scope * scope) {
	abort ();
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::REM>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createRem (nodes);
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::SUB>::defBuild (nir::Scope * scope) {
	auto nodes = resolveNodes (scope, args);
	if (nodes.empty ()) return nullptr;
	return scope->createSub (nodes);
}
