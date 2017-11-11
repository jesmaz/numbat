#pragma once


#include <ast/passes/shallnot.hpp>
#include <nir/instruction.hpp>


namespace AST {


class NirPass : public ShallNotPass {
	
	public:
		
		virtual NodePtr visit (const NodePtr & node) {return NirPass (scope) (node);}
		
		virtual void visit (const Basic_Operation & node);
		virtual void visit (const Call_0 & node);
		virtual void visit (const Call_1 & node);
		virtual void visit (const Call_2 & node);
		virtual void visit (const Call_n & node);
		virtual void visit (const IfElse & node);
		virtual void visit (const Sequence & node);
		virtual void visit (const Value & node);
		virtual void visit (const Variable & node);
		
		NirPass (nir::Scope * scope) : scope (scope) {}
		
	protected:
	private:
		
		const nir::Instruction * transform (const NodePtr & node) {inst = nullptr; node->accept (*this); assert (inst); return inst;}
		
		nir::Scope * scope;
		const nir::Instruction * inst;
		
};

class NirTypePass : public ShallNotPass {
	
	public:
		
		virtual NodePtr visit (const NodePtr & node) {return NirTypePass (scope) (node);}
		
		virtual void visit (const Array & node);
		virtual void visit (const Const & node);
		virtual void visit (const Inferred & node);
		virtual void visit (const Interface & node);
		virtual void visit (const Numeric & node);
		virtual void visit (const Ref & node);
		virtual void visit (const Struct & node);
		
		NirTypePass (nir::Scope * scope) : scope (scope) {}
		
		const nir::Type * transform (const AST::Type * t);
		
	protected:
	private:
		
		nir::Scope * scope;
		const nir::Type * type;
		
};


}
