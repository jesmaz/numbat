#pragma once


#include <ast/passes/analysis.hpp>


namespace AST {


class CallGraph : public AnalysisPass {
	
	public:
		
		virtual void analyse (const NodePtr & node) {CallGraph (*this) (node);}
		
		virtual void visit (const Call_0 & node);
		virtual void visit (const Call_1 & node);
		virtual void visit (const Call_2 & node);
		virtual void visit (const Call_n & node);
		
		CallGraph (const FuncPtr & activeFunc) : activeFunc (activeFunc) {}
		
	protected:
	private:
		
		FuncPtr activeFunc;
		
};


}
