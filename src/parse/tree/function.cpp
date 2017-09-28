#include <nir/parameter.hpp>
#include <nir/scope.hpp>
#include <parse/tree/function.hpp>


namespace parser {


AST::FuncPtr Function::createFunc (AST::Context & ctx) {
	
	if (fPtr) return fPtr;
	context = std::make_unique <AST::Context> (ctx);
	
	auto typeConv = [&](auto & a) {
		return a->createASTtype (*context);
	};
	
	fPtr = std::make_shared <AST::Function> ();
	fPtr->iden = iden;
	fPtr->params = params.map <AST::TypePtr> (typeConv);
	fPtr->retVals = type.map <AST::TypePtr> (typeConv);
	
	return fPtr;
}

AST::NodePtr Function::createAST (AST::Context & ctx) {
	
	auto func = createFunc (ctx);
	fPtr->body = body->createAST (*context);
	return std::make_shared <AST::Function_Ptr> (getPos (), ctx.getSourceFile (), std::move (func));
	
}

string Function::strDump (text::PrintMode mode) {
	
	string s = iden.empty () ? "" : "def ";
	s += mode&text::COLOUR ? text::blu + s + text::reset : s;
	s += iden.empty () ? "" : iden + " ";
	if (not tplate.empty ())
		s += "@[" + text::listPtrToStr (tplate.begin (), tplate.end (), mode) + "]";
	s += "(" + text::listPtrToStr (params.begin (), params.end (), mode) + ")";
	s += "->(" + text::listPtrToStr (type.begin (), type.end (), mode) + ")";
	s += body ? body->toString (mode) : "";
	return s;
	
}

Function::Function (PTNode iden, PTNode params, PTNode type, PTNode body) : ParseTreeNode (ParseTreeNode::NodeType::FUNCTION, (iden ? iden : params)->getPos ()) {
	if (iden){
		this->iden = iden->getIden ();
	}
	if (params) {
		if (params->isList ()) {
			this->params = params->getArgs ();
		} else {
			this->params = {params};
		}
	}
	if (type) {
		if (type->isList ()) {
			this->type = type->getArgs ();
		} else {
			this->type = {type};
		}
	}
	this->body = body;
}

Function::Function (numbat::lexer::position pos, const string & iden, const BasicArray <PTNode> & params, const BasicArray <PTNode> & type, PTNode body, nir::LINKAGE linkage) : ParseTreeNode (ParseTreeNode::NodeType::FUNCTION, pos), params (params), type (type), iden (iden), body (body), linkage (linkage) {}


}
