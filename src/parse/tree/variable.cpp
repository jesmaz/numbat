#include <ast/call.hpp>
#include <ast/variable.hpp>
#include <nir/instruction.hpp>
#include <nir/parameter.hpp>
#include <nir/scope.hpp>
#include <parse/tree/variable.hpp>
#include <utility/report.hpp>


namespace parser {


AST::NodePtr ParseTreeVariable::createAST (AST::Context & ctx) {
	
	AST::TypePtr type = vType->createASTtype (ctx);
	AST::VarPtr var = std::make_shared <AST::Variable> (getPos (), ctx.getSourceFile (), iden->getIden (), type);
	ctx.var (iden->getIden (), var);
	if (inst) {
		AST::NodePtr init = inst->createAST (ctx);
		return std::make_shared <AST::Unresolved_Constructor> (getPos (), ctx.getSourceFile (), var, BasicArray <AST::NodePtr> ({init}));
	} else {
		return std::make_shared <AST::Unresolved_Constructor> (getPos (), ctx.getSourceFile (), var, BasicArray <AST::NodePtr> ());
	}
	
}

AST::NodePtr ParseTreeVariable::createASTparam (AST::Context & ctx) {
	
	AST::TypePtr type = vType->createASTtype (ctx);
	AST::VarPtr var = std::make_shared <AST::Variable> (getPos (), ctx.getSourceFile (), iden->getIden (), type);
	if (inst) {
		AST::NodePtr init = inst->createAST (ctx);
		return std::make_shared <AST::Unresolved_Constructor> (getPos (), ctx.getSourceFile (), var, BasicArray <AST::NodePtr> ({init}));
	} else {
		return var;
	}
	
}

string ParseTreeVariable::strDump (text::PrintMode mode) {
	string s = "(" + vType->toString (mode) + " " + iden->toString (mode) + ")";
	if (inst) {
		s = "(" + s + " : " + inst->toString (mode) + ")";
	}
	return s;
}


}
