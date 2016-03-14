#ifndef PARSETREESLICE_HPP
#define PARSETREESLICE_HPP


#include "base.hpp"

#include <vector>


class ParseTreeSlice : public ParseTreeNode {
	
	public:
		
		virtual bool isAggregate () {return true;}
		virtual const std::vector <ParseTreeNode *> & getArgs () const {return args;}
		
		ParseTreeSlice (uint32_t line, uint32_t pos) : ParseTreeNode (line, pos) {}
		ParseTreeSlice (const std::vector <PTNode> & args) : ParseTreeNode (args.front ()->getLine (), args.front ()->getPos ()), args (args) {}
		virtual ~ParseTreeSlice () {for (PTNode n : args) delete n;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		std::vector <PTNode> args;
		
};


class ParseTreeSliceDecorator : public ParseTreeNode {
	
	public:
		
		ParseTreeSliceDecorator (PTNode decor, PTNode slice) : ParseTreeNode (decor->getLine (), decor->getPos ()), decor (decor), slice (slice) {}
		virtual ~ParseTreeSliceDecorator () {delete decor; delete slice;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		PTNode decor, slice;
		
};


#endif /*PARSETREESLICE_HPP*/