#pragma once


#include <ast/passes/identity.hpp>
#include <ast/passes/shallnot.hpp>


namespace AST {


class ResolvePass : public IdentityPass {
	
	public:
		
		virtual NodePtr visit (const NodePtr & node) {return ResolvePass () (node);}
		
		virtual void visit (const Unresolved_IfElse & node);
		virtual void visit (const Sequence & node);
		virtual void visit (const Unresolved_Call & node);
		virtual void visit (const Unresolved_Constructor & node);
		virtual void visit (const Unresolved_Operation & node);
		
	protected:
	private:
		
		
		
};


class MakeCallPass : public ResolvePass {
	
	public:
		
		virtual void visit (const Function_Ptr & node);
		virtual void visit (const Function_Set & node);
		virtual void visit (const Struct & node);
		virtual void visit (const Variable & node);
		
		MakeCallPass (numbat::lexer::position pos, const BasicArray <NodePtr> & args) : pos (pos), args (args) {}
		
		NodePtr operator () (const NodePtr & node);
		
	protected:
	private:
		
		numbat::lexer::position pos;
		const BasicArray <NodePtr> & args;
		
};

class CallMethodPass : public ShallNotPass {
	
	public:
		
		virtual void visit (const Array & node);
		virtual void visit (const Const & node);
		virtual void visit (const Interface & node);
		virtual void visit (const Numeric & node);
		virtual void visit (const Ref & node);
		virtual void visit (const Struct & node);
		
		CallMethodPass (numbat::lexer::position pos, const BasicArray <NodePtr> & args) : defaultAction (parser::OPERATION::NONE), pos (pos), args (args) {}
		CallMethodPass (numbat::lexer::position pos, parser::OPERATION defaultAction, const BasicArray <NodePtr> & args) : defaultAction (defaultAction), pos (pos), args (args) {}
		
		//NodePtr operator () (const NodePtr & node);
		
	protected:
	private:
		
		parser::OPERATION defaultAction;
		numbat::lexer::position pos;
		const BasicArray <NodePtr> & args;
		
};

class ConstructorSelectionPass : public ShallNotPass {
	
	public:
		
		virtual NodePtr visit (const NodePtr & node) {return ConstructorSelectionPass (*this) (node);}
		
		virtual void visit (const Array& node);
		virtual void visit (const ArrayInit & node);
		virtual void visit (const Const & node);
		virtual void visit (const Inferred & node);
		virtual void visit (const Interface & node);
		virtual void visit (const Numeric & node);
		virtual void visit (const Ref & node);
		virtual void visit (const Struct & node);
		
		ConstructorSelectionPass (numbat::lexer::position pos, const VarPtr & var, const BasicArray <NodePtr> & args) : pos (pos), var (var), args (args) {}
		
		NodePtr operator () (const NodePtr & node);
		
	protected:
	private:
		
		numbat::lexer::position pos;
		const VarPtr & var;
		const BasicArray <NodePtr> & args;
		TypePtr replacementType = nullptr;
		
};


}
