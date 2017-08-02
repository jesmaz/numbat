#pragma once


#include <ast/passes/identity.hpp>
#include <ast/passes/shallnot.hpp>


namespace AST {


class MetaTagSwizzlePass : public IdentityPass {
	
	public:
		
		virtual NodePtr visit (const NodePtr & node) {return MetaTagSwizzlePass () (node);}
		
		virtual void visit (const Reflect & node);
		
	protected:
	private:
		
		
		
};


class SwizzleSubjectPass : public ShallNotPass {
	
	public:
		
		virtual NodePtr visit (const NodePtr & node) {return MetaTagSwizzlePass () (node);}
		
		virtual void visit (const Unresolved_Constructor & node);
		virtual void visit (const Variable & node);
		
		SwizzleSubjectPass (const NodePtr & metaTag) : metaTag (metaTag) {}
		
	protected:
	private:
		
		NodePtr metaTag;
		
};


}

