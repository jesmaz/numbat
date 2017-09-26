#pragma once


#include <forward.hpp>


namespace AST {


class And;
class Array;
class ArrayInit;
class Basic_Operation;
class Call_0;
class Call_1;
class Call_2;
class Call_n;
class CastToArbPrec;
class CastToInt;
class CastToDecimal;
class CastToFloat;
class CastToUint;
class Const;
class Function_Ptr;
class Function_Set;
class IfElse;
class Inferred;
class Interface;
class Number;
class Numeric;
class Or;
class Ref;
class Reflect;
class ReflectType;
class Sequence;
class Struct;
class Unresolved_Call;
class Unresolved_Constructor;
class Unresolved_IfElse;
class Unresolved_Operation;
class Variable;

class AbstractPass {
	
	public:
		
		virtual NodePtr visit (const NodePtr & node)=0;
		
		virtual void visit (const And & node)=0;
		virtual void visit (const Array & node)=0;
		virtual void visit (const ArrayInit & node)=0;
		virtual void visit (const Basic_Operation & node)=0;
		virtual void visit (const Call_0 & node)=0;
		virtual void visit (const Call_1 & node)=0;
		virtual void visit (const Call_2 & node)=0;
		virtual void visit (const Call_n & node)=0;
		virtual void visit (const CastToArbPrec & node)=0;
		virtual void visit (const CastToInt & node)=0;
		virtual void visit (const CastToDecimal & node)=0;
		virtual void visit (const CastToFloat & node)=0;
		virtual void visit (const CastToUint & node)=0;
		virtual void visit (const Const & node)=0;
		virtual void visit (const Function_Ptr & node)=0;
		virtual void visit (const Function_Set & node)=0;
		virtual void visit (const IfElse & node)=0;
		virtual void visit (const Inferred & node)=0;
		virtual void visit (const Interface & node)=0;
		virtual void visit (const Number & node)=0;
		virtual void visit (const Numeric & node)=0;
		virtual void visit (const Or & node)=0;
		virtual void visit (const Ref & node)=0;
		virtual void visit (const Reflect & node)=0;
		virtual void visit (const ReflectType & node)=0;
		virtual void visit (const Sequence & node)=0;
		virtual void visit (const Struct & node)=0;
		virtual void visit (const Unresolved_Call & node)=0;
		virtual void visit (const Unresolved_Constructor & node)=0;
		virtual void visit (const Unresolved_IfElse & node)=0;
		virtual void visit (const Unresolved_Operation & node)=0;
		virtual void visit (const Variable & node)=0;
		
		NodePtr operator () (const NodePtr & node);
		
	protected:
		
		NodePtr nPtr;
		
	private:
		
		
		
};


}
