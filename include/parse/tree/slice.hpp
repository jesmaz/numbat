#ifndef PARSETREESLICE_HPP
#define PARSETREESLICE_HPP


#include "base.hpp"

 


class ParseTreeSlice : public ParseTreeNode {
	
	public:
		
		ParseTreeSlice (numbat::lexer::position pos) : ParseTreeNode (pos) {}
		ParseTreeSlice (numbat::lexer::position pos, const PTNode & start, const PTNode & end, const PTNode & step) : ParseTreeNode (pos), start (start), end (end), step (step) {}
		virtual ~ParseTreeSlice () {delete start; delete end; delete step;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		PTNode start, end, step;
		
};


class ParseTreeSliceDecorator : public ParseTreeNode {
	
	public:
		
		ParseTreeSliceDecorator (PTNode decor, PTNode slice) : ParseTreeNode (decor->getPos ()), decor (decor), slice (slice) {}
		virtual ~ParseTreeSliceDecorator () {delete decor; delete slice;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		PTNode decor, slice;
		
};


class ParseTreeSliceForEach : public ParseTreeNode {
	
	public:
		
		ParseTreeSliceForEach (numbat::lexer::position pos, const string & var, PTNode range) : ParseTreeNode (pos), var (var), range (range) {}
		virtual ~ParseTreeSliceForEach () {delete range;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		string var;
		PTNode range;
		
};


#endif /*PARSETREESLICE_HPP*/
