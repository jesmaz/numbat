#pragma once


#include <ast/function.hpp>
#include <ast/node.hpp>
#include <ast/type.hpp>


namespace AST {

class Reflect : public Node {
	
	public:
		
		static FuncPtr api (const string & iden);
		static void initAPI ();
		
		NodePtr getMetaTag () const {return metaTag;}
		NodePtr getTarget () const {return target;}
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		Reflect (numbat::lexer::position pos, const NodePtr & metaTag, const NodePtr & target) : Node (pos), metaTag (metaTag), target (target) {}
		
	protected:
	private:
		
		NodePtr metaTag, target;
		static std::map <string, FuncPtr> apiFuncs;
		
};

class ReflectType : public Type {
	
	public:
		
		NodePtr getMetaTag () const {return metaTag;}
		TypePtr getTarget () const {return target;}
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		static TypePtr get (const NodePtr & metaTag, const TypePtr & target);
		
	protected:
	private:
		
		ReflectType (const NodePtr & metaTag, const TypePtr & target) : Type (metaTag->getPos ()), metaTag (metaTag), target (target) {}
		
		NodePtr metaTag;
		TypePtr target;
		static std::map <std::pair <Node*, Type*>, TypePtr> typeMap;
		
};

}
