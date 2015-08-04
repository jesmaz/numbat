#ifndef PARSETREE_HPP
#define PARSETREE_HPP


#include "tree/function.hpp"
#include "tree/struct.hpp"

#include <vector>


class ParseTree : public ParseTreeNode {
	
	public:
		
		const std::vector <Function *> & getFunctions () const {return functions;}
		const std::vector <PTNode> & getBody () const {return body;}
		const std::vector <Struct *> & getStructs () const {return structs;}
		
		numbat::parser::ASTnode build (numbat::parser::NumbatScope * scope);
		
		ParseTree (uint32_t line, uint32_t pos) : ParseTreeNode (line, pos) {}
		ParseTree (const std::vector <PTNode> & body) : ParseTreeNode (body.front ()->getLine (), body.front ()->getPos ()), body (body) {}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) {if (body.size ()) {string s = "{"; for (size_t i=0; i+1<body.size (); ++i) s += body [i]->toString () + "; "; return s + body.back ()->toString () + ";}";} else {return "{}";}}
		
		std::vector <PTNode> body;
		std::vector <Function *> functions;
		std::vector <Struct *> structs;
		
};


#endif /*PARSERTREE_HPP*/