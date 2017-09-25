#pragma once


#include <ast/passes/identity.hpp>
#include <ast/passes/shallnot.hpp>
#include <map>
#include <memory>


namespace AST {


class ReflectPass : public IdentityPass {
	
	public:
		
		virtual NodePtr visit (const NodePtr & node) {return ReflectPass () (node);}
		
		virtual void visit (const Reflect & node);
		virtual void visit (const ReflectType & node);
		virtual void visit (const Variable & node);
		
	protected:
	private:
		
		
		
};


}


