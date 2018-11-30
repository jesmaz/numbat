#pragma once


#include <ast/passes/analysis.hpp>
#include <ast/passes/shallnot.hpp>


namespace AST {


class FunctionReturnsPass : public AnalysisPass {
	
	public:
		
		const bool funcReturns () {return returns;}
		
		virtual void analyse (const NodePtr & node) {FunctionReturnsPass (*this) (node);}
		
		virtual void visit (const And & node);
		virtual void visit (const IfElse & node);
		virtual void visit (const Or & node);
		virtual void visit (const Return & node);
		virtual void visit (const Sequence & node);
		
	protected:
	private:
		
		bool returns = false;
		
};


class InsertReturnPass : public ShallNotPass {
	
	public:
		
		virtual NodePtr visit (const NodePtr & node) {return InsertReturnPass (*this) (node);}
		
		virtual void visit (const IfElse & node);
		virtual void visit (const Sequence & node);
		virtual void visit (const SystemCall & node);
		
		InsertReturnPass (const FuncPtr & func) : func (func) {}
		
	protected:
	private:
		
		void defAction (const Node & node);
		
		FuncPtr func;
		
};


}

