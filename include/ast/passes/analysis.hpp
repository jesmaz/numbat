#pragma once


#include <ast/passes/abstract.hpp>


namespace AST {


class AnalysisPass : public AbstractPass {
	
	public:
		
		virtual NodePtr visit (const NodePtr & node) {this->analyse (node); return nullptr;}
		virtual void analyse (const NodePtr & node) {node->accept (*this);}
		
		virtual void visit (const And & node);
		virtual void visit (const Array & node);
		virtual void visit (const ArrayInit & node);
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
		virtual void visit (const Const & node);
		virtual void visit (const Function_Ptr & node);
		virtual void visit (const Function_Set & node);
		virtual void visit (const IfElse & node);
		virtual void visit (const Import & node);
		virtual void visit (const Inferred & node);
		virtual void visit (const Interface & node);
		virtual void visit (const Numeric & node);
		virtual void visit (const Or & node);
		virtual void visit (const RawInit & node);
		virtual void visit (const Ref & node);
		virtual void visit (const Reflect & node);
		virtual void visit (const ReflectType & node);
		virtual void visit (const Return & node);
		virtual void visit (const Sequence & node);
		virtual void visit (const StaticIndex & node);
		virtual void visit (const StaticValue & node);
		virtual void visit (const Struct & node);
		virtual void visit (const SystemCall & node);
		virtual void visit (const Unresolved_Call & node);
		virtual void visit (const Unresolved_Constructor & node);
		virtual void visit (const Unresolved_Get_Member & node);
		virtual void visit (const Unresolved_IfElse & node);
		virtual void visit (const Unresolved_Operation & node);
		virtual void visit (const Variable & node);
		
	protected:
	private:
		
		
		
};


}

