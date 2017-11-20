#pragma once


#include <ast/passes/shallnot.hpp>
#include <utility/literalstack.hpp>


namespace AST {


class FoldConstPass : public ShallNotPass {
	
	public:
		
		virtual NodePtr visit (const NodePtr & node) {return FoldConstPass (*this) (node);}
		virtual NodePtr visit (const NodePtr & node, bool readVar) {return FoldConstPass (readVar) (node);}
		virtual NodePtr visit (const NodePtr & node, bool readVar, const std::shared_ptr <LiteralStack> & executionStack) {return FoldConstPass (readVar, executionStack) (node);}
		
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
		virtual void visit (const RawInit & node);
		virtual void visit (const Sequence & node);
		virtual void visit (const StaticIndex &);
		virtual void visit (const Struct &) {}
		virtual void visit (const Value &) {}
		virtual void visit (const Variable &);
		
		FoldConstPass (bool readVar=true, const std::shared_ptr <LiteralStack> & stack=nullptr) : readVar (readVar), executionStack (stack) {
			if (not executionStack) {
				executionStack = std::make_shared <LiteralStack> ();
			}
		}
		
	protected:
	private:
		
		bool readVar=true;
		std::shared_ptr <LiteralStack> executionStack;
		
};


class ExecutePass : public FoldConstPass {
	
	public:
		
		virtual NodePtr visit (const NodePtr & node) {return ExecutePass (*this) (node);}
		virtual NodePtr visit (const NodePtr & node, bool readVar) {return ExecutePass (readVar) (node);}
		virtual NodePtr visit (const NodePtr & node, bool readVar, const std::shared_ptr <LiteralStack> & executionStack) {return ExecutePass (readVar, executionStack) (node);}
		
		ExecutePass (bool readVar=true, const std::shared_ptr <LiteralStack> & stack=nullptr) : FoldConstPass (readVar, stack) {}
		
	protected:
	private:
		
		
		
};


}
