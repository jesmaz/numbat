#ifndef CALL_HPP
#define CALL_HPP


#include "base.hpp"

 


class ParseTreeCall : public ParseTreeNode {
	
	public:
		
		
		virtual bool isAggregate () {return true;}
		virtual const BasicArray <ParseTreeNode *> & getArgs () const {return args;}
		
		virtual const nir::Instruction * build (nir::Scope * scope);
		
		ParseTreeCall (uint32_t line, uint32_t pos) : ParseTreeNode (line, pos) {}
		ParseTreeCall (const PTNode & iden, const BasicArray <PTNode> & args) : ParseTreeNode (iden->getLine (), iden->getPos ()), iden (iden), args (args) {}
		virtual ~ParseTreeCall () {for (PTNode n : args) delete n;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		PTNode iden;
		BasicArray <PTNode> args;
		
};


#endif /*CALL_HPP*/
