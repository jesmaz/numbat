#pragma once


#include <ast/passes/shallnot.hpp>


namespace AST {


class DominantType : public ShallNotPass {
	
	public:
		
		virtual NodePtr visit (const NodePtr & node) {abort ();}
		
		virtual void visit (const Array & node);
		virtual void visit (const Numeric & node);
		virtual void visit (const ReflectType & node);
		virtual void visit (const Struct & node);
		
		
		DominantType (const TypePtr & a, const TypePtr & b) : a (a), b (b) {}
		
		TypePtr operator () ();
		
	protected:
	private:
		
		NodePtr operator () (const NodePtr & node) {abort ();}
		
		TypePtr a, b;
		
};


}
