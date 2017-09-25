#pragma once


#include <ast/passes/shallnot.hpp>


namespace AST {


class FoldConstPass : public ShallNotPass {
	
	public:
		
		virtual NodePtr visit (const NodePtr & node) {return FoldConstPass () (node);}
		
		virtual void visit (const Basic_Operation & node);
		virtual void visit (const Call_0 & node);
		virtual void visit (const Call_1 & node);
		virtual void visit (const Call_2 & node);
		virtual void visit (const Call_n & node);
		virtual void visit (const Number &) {}
		virtual void visit (const Sequence & node);
		
	protected:
	private:
		
		
		
};


class ExecutePass : public FoldConstPass {
	
	public:
		
		virtual NodePtr visit (const NodePtr & node) {return ExecutePass () (node);}
		
	protected:
	private:
		
		
		
};


}
