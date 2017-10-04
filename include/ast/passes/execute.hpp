#pragma once


#include <ast/passes/shallnot.hpp>


namespace AST {


class FoldConstPass : public ShallNotPass {
	
	public:
		
		virtual NodePtr visit (const NodePtr & node) {return FoldConstPass (*this) (node);}
		
		virtual void visit (const ArrayVal &) {}
		virtual void visit (const Basic_Operation & node);
		virtual void visit (const Call_0 & node);
		virtual void visit (const Call_1 & node);
		virtual void visit (const Call_2 & node);
		virtual void visit (const Call_n & node);
		virtual void visit (const CastToArbPrec & node);
		virtual void visit (const CastToDecimal & node);
		virtual void visit (const CastToFloat & node);
		virtual void visit (const CastToInt & node);
		virtual void visit (const CastToUint & node);
		virtual void visit (const Function_Ptr &) {}
		virtual void visit (const Number &) {}
		virtual void visit (const Sequence & node);
		virtual void visit (const Struct &) {}
		virtual void visit (const Variable &);
		
		FoldConstPass (bool readVar=true) : readVar (readVar) {}
		
	protected:
	private:
		
		bool readVar=true;
		
};


class ExecutePass : public FoldConstPass {
	
	public:
		
		virtual NodePtr visit (const NodePtr & node) {return ExecutePass (*this) (node);}
		
		ExecutePass (bool readVar=true) : FoldConstPass (readVar) {}
		
	protected:
	private:
		
		
		
};


}
