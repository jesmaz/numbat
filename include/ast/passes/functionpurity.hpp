#pragma once


#include <ast/passes/analysis.hpp>
#include <set>


namespace AST {


class FunctionPurityPass : public AnalysisPass {
	
	public:
		
		virtual void analyse (const NodePtr & node) {node->accept (*this);}
		
		virtual void visit (const Call_0 & node);
		virtual void visit (const Call_1 & node);
		virtual void visit (const Call_2 & node);
		virtual void visit (const Call_n & node);
		virtual void visit (const Function_Ptr & node);
		virtual void visit (const Function_Set & node);
		virtual void visit (const VariableRef & node);
		
		static void analyse (const std::set <FuncPtr> & funcs);
		
	protected:
	private:
		
		bool operator ()();
		
		FunctionPurityPass (const FuncPtr & activeFunc) : activeFunc (activeFunc) {}
		
		FuncPtr activeFunc;
		
};


}
