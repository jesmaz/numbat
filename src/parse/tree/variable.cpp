#include <ast/call.hpp>
#include <ast/variable.hpp>
#include <nir/instruction.hpp>
#include <nir/parameter.hpp>
#include <nir/scope.hpp>
#include <parse/tree/variable.hpp>
#include <utility/report.hpp>


namespace parser {


AST::NodePtr ParseTreeVariable::createAST (AST::Context & ctx) {
	
	AST::TypePtr type = createASTtype (ctx);
	size_t pos;
	AST::Variable::LOCATION loc;
	if (ctx.getOwner ()) {
		// Local var
		pos = ctx.allocStack ();
		loc = AST::Variable::LOCATION::LOCAL;
	} else {
		// Globlal var
		pos = AST::Variable::globalContex.reserve ();
		loc = AST::Variable::LOCATION::GLOBAL;
	}
	AST::VarPtr var = std::make_shared <AST::Variable> (getPos (), ctx.getSourceFile (), type, pos, loc, iden->getIden ());
	ctx.var (iden->getIden (), var);
	if (inst) {
		AST::NodePtr init = inst->createAST (ctx);
		return std::make_shared <AST::Unresolved_Constructor> (getPos (), ctx.getSourceFile (), var, BasicArray <AST::NodePtr> ({init}));
	} else {
		return std::make_shared <AST::Unresolved_Constructor> (getPos (), ctx.getSourceFile (), var, BasicArray <AST::NodePtr> ());
	}
	
}

AST::NodePtr ParseTreeVariable::createASTparam (AST::Context & ctx) {
	
	AST::TypePtr type = createASTtype (ctx);
	size_t pos;
	AST::Variable::LOCATION loc;
	if (ctx.getOwner ()) {
		// Local var
		pos = ctx.allocStack ();
		loc = AST::Variable::LOCATION::LOCAL;
	} else {
		// Globlal var
		pos = AST::Variable::globalContex.reserve ();
		loc = AST::Variable::LOCATION::GLOBAL;
	}
	AST::VarPtr var = std::make_shared <AST::Variable> (getPos (), ctx.getSourceFile (), type, pos, loc, iden->getIden ());
	if (inst) {
		AST::NodePtr init = inst->createAST (ctx);
		return std::make_shared <AST::Unresolved_Constructor> (getPos (), ctx.getSourceFile (), var, BasicArray <AST::NodePtr> ({init}));
	} else {
		return var;
	}
	
}

AST::TypePtr ParseTreeVariable::createASTtype (AST::Context & ctx) {
	return vType->createASTtype (ctx);
}

string ParseTreeVariable::strDump (text::PrintMode mode) {
	string s = "(" + vType->toString (mode) + " " + iden->toString (mode) + ")";
	if (inst) {
		s = "(" + s + " : " + inst->toString (mode) + ")";
	}
	return s;
}


}
