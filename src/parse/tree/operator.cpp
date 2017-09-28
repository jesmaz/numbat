#include <ast/flowcontrol.hpp>
#include <ast/variable.hpp>
#include <nir/scope.hpp>
#include <nir/type/array.hpp>
#include <parse/tree/operator.hpp>
#include <utility/report.hpp>


namespace parser {


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
AST::NodePtr SpecificOperator <OPERATION::AND>::createAST (AST::Context & ctx) {
	if (args.size () != 2) {
		report::logMessage (report::ERROR, ctx.getSourceFile (), getPos (), "Operator '" + iden + "' must have 2 arguments");
		return nullptr;
		
	} else {
		auto params = args.map <AST::NodePtr> ([&](auto & a) {return a->createAST (ctx);});
		return std::make_shared <AST::And> (getPos (), ctx.getSourceFile (), params [0], params [1]);
	}
}

template <>
AST::NodePtr SpecificOperator <OPERATION::AS>::createAST (AST::Context & ctx) {
	if (args.size () != 2) {
		report::logMessage (report::ERROR, ctx.getSourceFile (), getPos (), "Operator '" + iden + "' must have 2 arguments");
		return nullptr;
		
	} else {
		BasicArray <AST::NodePtr> params = {args [0]->createAST (ctx), args [1]->createASTtype (ctx)};
		return std::make_shared <AST::Unresolved_Operation> (
			getPos (),
			ctx.getSourceFile (), 
			iden, 
			params,
			OPERATION::AS
		);
		
	}
}

template <>
AST::NodePtr SpecificOperator <OPERATION::OR>::createAST (AST::Context & ctx) {
	if (args.size () != 2) {
		report::logMessage (report::ERROR, ctx.getSourceFile (), getPos (), "Operator '" + iden + "' must have 2 arguments");
		return nullptr;
		
	} else {
		auto params = args.map <AST::NodePtr> ([&](auto & a) {return a->createAST (ctx);});
		return std::make_shared <AST::Or> (getPos (), ctx.getSourceFile (), params [0], params [1]);
	}
}


bool isPredicate (OPERATION opp) {
	switch (opp) {
		case OPERATION::ADD:
		case OPERATION::AND:
		case OPERATION::AS:
		case OPERATION::ASSIGN:
		case OPERATION::BAND:
		case OPERATION::BNOT:
		case OPERATION::BOR:
		case OPERATION::BXOR:
			return false;
		case OPERATION::CMPEQ:
		case OPERATION::CMPGT:
		case OPERATION::CMPGTE:
		case OPERATION::CMPLT:
		case OPERATION::CMPLTE:
		case OPERATION::CMPNE:
			return true;
		case OPERATION::CONCAT:
		case OPERATION::DECREMENT:
		case OPERATION::DIV:
		case OPERATION::IN:
		case OPERATION::INCREMENT:
		case OPERATION::INDEX:
			return false;
		case OPERATION::LNOT:
			return true;
		case OPERATION::MUL:
		case OPERATION::NEG:
		case OPERATION::NONE:
		case OPERATION::OR:
		case OPERATION::REM:
		case OPERATION::SUB:
		case OPERATION::__COUNT__:
			return false;
	}
}


}
