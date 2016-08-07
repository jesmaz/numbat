#include <nir/scope.hpp>
#include <parse/tree/operator.hpp>


std::vector <nir::Argument> resolveNodes (nir::Scope * scope, ParseTreeNode::BuildMode mode, std::vector <PTNode> args) {
	
	std::vector <nir::Argument> nodes;
	nodes.resize (args.size (), {nullptr, nullptr});
	//std::vector <numbat::parser::FunctionDecleration *> candidates;
	for (size_t i=0; i<args.size(); ++i) {
		assert (args[i]);
		const nir::Instruction * instr = args[i]->build (scope, mode);
		nodes [i] = {instr, instr->getIden ()};
		assert (instr);
	}
	
	return nodes;
	
}


const nir::Instruction * GenericOperator::build (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	return defBuild (scope, mode);
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
const nir::Instruction * SpecificOperator <OPERATION::ADD>::defBuild (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	return scope->createAdd (resolveNodes (scope, mode, args));
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::AND>::defBuild (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	abort ();
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::AS>::defBuild (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	abort ();
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::ASSIGN>::defBuild (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	return scope->createAssign (resolveNodes (scope, mode, args));
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::BAND>::defBuild (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	return scope->createBitAnd (resolveNodes (scope, mode, args));
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::BNOT>::defBuild (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	return scope->createBitNot (resolveNodes (scope, mode, args));
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::BOR>::defBuild (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	return scope->createBitOr (resolveNodes (scope, mode, args));
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::BXOR>::defBuild (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	return scope->createBitXor (resolveNodes (scope, mode, args));
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::CMPEQ>::defBuild (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	abort ();
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::CMPGT>::defBuild (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	return scope->createCmpGT (resolveNodes (scope, mode, args));
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::CMPGTE>::defBuild (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	abort ();
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::CMPLT>::defBuild (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	return scope->createCmpLT (resolveNodes (scope, mode, args));
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::CMPLTE>::defBuild (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	abort ();
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::CMPNE>::defBuild (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	abort ();
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::CONCAT>::defBuild (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	abort ();
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::DECREMENT>::defBuild (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	abort ();
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::DIV>::defBuild (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	return scope->createDiv (resolveNodes (scope, mode, args));
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::IN>::defBuild (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	abort ();
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::INCREMENT>::defBuild (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	abort ();
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::LNOT>::defBuild (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	return scope->createLNot (resolveNodes (scope, mode, args));
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::MUL>::defBuild (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	return scope->createMul (resolveNodes (scope, mode, args));
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::NEG>::defBuild (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	return scope->createNeg (resolveNodes (scope, mode, args));
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::NONE>::defBuild (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	abort ();
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::OR>::defBuild (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	abort ();
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::REM>::defBuild (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	abort ();
}

template <>
const nir::Instruction * SpecificOperator <OPERATION::SUB>::defBuild (nir::Scope * scope, ParseTreeNode::BuildMode mode) {
	return scope->createSub (resolveNodes (scope, mode, args));
}