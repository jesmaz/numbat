#include <ast/call.hpp>
#include <ast/passes/shallnot.hpp>
#include <ast/passes/typeutil.hpp>
#include <ast/variable.hpp>
#include <parse/tree/function.hpp>


namespace parser {


class SeedParameterPass : public AST::ShallNotPass {
	
	public:
		
		virtual AST::NodePtr visit (const AST::NodePtr & node) {return SeedParameterPass (*this) (node);}
		
		virtual void visit(const AST::Variable & node) {
			positions [node.getIden ()] = pos;
			tPtr = node.getType ();
			ctx.var (node.getIden (), nPtr);
		}
		virtual void visit (const AST::Unresolved_Constructor & node) {
			visit (*node.getVar ());
			defaults [pos] = nPtr;
		}
		
		SeedParameterPass (BasicArray <AST::NodePtr> & defaults, std::map <std::string, size_t> & positions, size_t pos, AST::Context & ctx) : defaults (defaults), positions (positions), pos (pos), ctx (ctx) {}
		
		AST::TypePtr getType (const AST::NodePtr & node) {
			nPtr = node;
			if (not (tPtr = std::dynamic_pointer_cast <AST::Type> (node))) {
				node->accept (*this);
			}
			return tPtr;
		}
		
	protected:
	private:
		
		BasicArray <AST::NodePtr> & defaults;
		std::map <std::string, size_t> & positions;
		size_t pos;
		AST::Context & ctx;
		AST::TypePtr tPtr;
		
};


AST::FuncPtr Function::createFunc (AST::Context & ctx) {
	
	if (fPtr) return fPtr;
	
	fPtr = std::make_shared <AST::Function> ();
	
	context = std::make_unique <AST::Context> (ctx, fPtr.get ());
	
	fPtr->iden = iden;
	fPtr->defParams = BasicArray <AST::NodePtr> (params.size ());
	fPtr->params = params.map <AST::TypePtr> ([&](auto & a) {
		static size_t index=0;
		return SeedParameterPass (fPtr->defParams, fPtr->positions, index++, *context).getType (a->createASTparam (*context));
	});
	fPtr->defRets = BasicArray <AST::NodePtr> (type.size ());
	fPtr->retVals = type.map <AST::TypePtr> ([&](auto & a) {
		static size_t index=0;
		return SeedParameterPass (fPtr->defRets, fPtr->positions, index++, *context).getType (a->createASTparam (*context));
	});
	
	ctx.func (iden, fPtr);
	
	return fPtr;
}

AST::NodePtr Function::createAST (AST::Context & ctx) {
	
	auto func = createFunc (ctx);
	auto type = ctx.resolveType (iden);
	if (type) {
		context->var ("this", std::make_shared <AST::Variable> (getPos (), ctx.getSourceFile (), type, context->allocStack (), AST::Variable::LOCATION::LOCAL, "this"));
		type->overloadFunc ("", func);
	}
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
