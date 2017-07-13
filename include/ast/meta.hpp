#pragma once


#include <ast/function.hpp>
#include <ast/node.hpp>
#include <ast/type.hpp>


namespace AST {

class Reflect : public Node {
	
	public:
		
		static FuncPtr api (const string & iden);
		
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

}
