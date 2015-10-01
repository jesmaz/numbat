#ifndef CALL_HPP
#define CALL_HPP


#include "base.hpp"

#include <vector>


class ParseTreeCall : public ParseTreeNode {
	
	public:
		
		virtual bool isAggregate () {return true;}
		virtual const std::vector <ParseTreeNode *> & getArgs () const {return args;}
		
		virtual numbat::parser::ASTnode build (numbat::parser::NumbatScope * scope);
		
		ParseTreeCall (uint32_t line, uint32_t pos) : ParseTreeNode (line, pos) {}
		ParseTreeCall (const PTNode & iden, const std::vector <PTNode> & args) : ParseTreeNode (iden->getLine (), iden->getPos ()), iden (iden), args (args) {}
		virtual ~ParseTreeCall () {for (PTNode n : args) delete n;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		PTNode iden;
		std::vector <PTNode> args;
		
};


#endif /*CALL_HPP*/