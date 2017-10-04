#pragma once


#include <ast/passes/shallnot.hpp>
#include <nir/instruction.hpp>


namespace AST {


class PruneDeadCodePass : public ShallNotPass {
	
	public:
		
		virtual NodePtr visit (const NodePtr & node) {return PruneDeadCodePass () (node);}
		
		virtual void visit (const ArrayVal &) {nPtr = nullptr;}
		virtual void visit (const Basic_Operation &) {}
		virtual void visit (const Call_0 &) {}
		virtual void visit (const Call_1 &) {}
		virtual void visit (const Call_2 &) {}
		virtual void visit (const Call_n &) {}
		virtual void visit (const CastToArbPrec &) {nPtr = nullptr;}
		virtual void visit (const CastToDecimal &) {nPtr = nullptr;}
		virtual void visit (const CastToFloat &) {nPtr = nullptr;}
		virtual void visit (const CastToInt &) {nPtr = nullptr;}
		virtual void visit (const CastToUint &) {nPtr = nullptr;}
		virtual void visit (const Function_Ptr &) {nPtr = nullptr;}
		virtual void visit (const Number &) {nPtr = nullptr;}
		virtual void visit (const Sequence & seq);
		virtual void visit (const Struct &) {nPtr = nullptr;}
		virtual void visit (const Variable &) {nPtr = nullptr;}
		
		NodePtr operator () (const NodePtr & node);
		
	protected:
	private:
		
};

}
