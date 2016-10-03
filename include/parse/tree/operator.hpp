#pragma once


#include <parse/tree/base.hpp>
 


enum class OPERATION {ADD, AND, AS, ASSIGN, BAND, BNOT, BOR, BXOR, CMPEQ, CMPGT, CMPGTE, CMPLT, CMPLTE, CMPNE, CONCAT, DECREMENT, DIV, IN, INCREMENT, LNOT, MUL, NEG, NONE, OR, REM, SUB};


class GenericOperator : public ParseTreeNode {
	
	public:
		
		virtual bool isAggregate () {return true;}
		virtual const BasicArray <ParseTreeNode *> & getArgs () const {return args;}
		virtual const nir::Instruction * build (nir::Scope * scope);
		
		GenericOperator (numbat::lexer::position pos) : ParseTreeNode (pos) {}
		GenericOperator (const string & iden, std::initializer_list <PTNode> args) : ParseTreeNode ((*args.begin ())->getPos ()), iden (iden), args (args) {}
		virtual ~GenericOperator () {delAll (args);}
		
	protected:
		
		virtual const nir::Instruction * defBuild (nir::Scope * scope)=0;
		
		string iden;
		BasicArray <PTNode> args;
		
	private:
		
		virtual string strDump (text::PrintMode mode);
		
};

template <OPERATION opp>
class SpecificOperator : public GenericOperator {
	
	public:
		
		SpecificOperator (numbat::lexer::position pos) : GenericOperator (pos) {}
		SpecificOperator (const string & iden, std::initializer_list <PTNode> args) : GenericOperator (iden, args) {}
		
	protected:
		
		virtual const nir::Instruction * defBuild (nir::Scope * scope);
		
	private:
};
