#pragma once


#include <ast/passes/analysis.hpp>


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


}

