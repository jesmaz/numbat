#pragma once


#include <ast/passes/abstract.hpp>
#include <ast/type.hpp>


namespace AST {


class IdentityPass : public AbstractPass {
	
	public:
		
		virtual NodePtr visit (const NodePtr & node) {return IdentityPass () (node);}
		
		virtual void visit (const And & node);
		virtual void visit (const Array & node);
		virtual void visit (const ArrayInit & node);
		virtual void visit (const Basic_Operation & node);
		virtual void visit (const Call_0 & node);
		virtual void visit (const Call_1 & node);
		virtual void visit (const Call_2 & node);
		virtual void visit (const Call_n & node);
		virtual void visit (const Const & node);
		virtual void visit (const Function_Ptr & node);
		virtual void visit (const Function_Set & node);
		virtual void visit (const IfElse & node);
		virtual void visit (const Inferred & node);
		virtual void visit (const Interface & node);
		virtual void visit (const Number & node);
		virtual void visit (const Numeric & node);
		virtual void visit (const Or & node);
		virtual void visit (const Ref & node);
		virtual void visit (const Reflect & node);
		virtual void visit (const ReflectType & node);
		virtual void visit (const Sequence & node);
		virtual void visit (const Struct & node);
		virtual void visit (const Unresolved_Call & node);
		virtual void visit (const Unresolved_Constructor & node);
		virtual void visit (const Unresolved_IfElse & node);
		virtual void visit (const Unresolved_Operation & node);
		virtual void visit (const Variable & node);
		
	protected:
	private:
		
		
		
};


}
