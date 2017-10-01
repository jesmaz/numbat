#pragma once


#include <ast/passes/shallnot.hpp>
#include <ast/type.hpp>


namespace AST {


class CastToNumberPass : public ShallNotPass {
	
	public:
		
		virtual NodePtr visit (const NodePtr & node) {return CastToNumberPass (*this) (node);}
		
		virtual void visit (const Array & node);
		virtual void visit (const ArrayInit & node);
		virtual void visit (const Const & node);
		virtual void visit (const Inferred & node);
		virtual void visit (const Interface & node);
		virtual void visit (const Numeric & node);
		virtual void visit (const Ref & node);
		virtual void visit (const ReflectType & node);
		virtual void visit (const Struct & node);
		
		CastToNumberPass (const Numeric & numeric) : numeric (numeric) {}
		
		NodePtr operator () (const NodePtr & node);
		
	protected:
	private:
		
		const Numeric & numeric;
		
};

class StaticCastPass : public ShallNotPass {
	
	public:
		
		virtual NodePtr visit (const NodePtr & node) {return StaticCastPass (*this) (node);}
		
		StaticCastPass (const TypePtr & target) : target (target) {}
		
		NodePtr operator () (const NodePtr & node);
		
	protected:
	private:
		
		const TypePtr & target;
		
};


}




