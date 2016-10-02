#pragma once


#include <parse/tree/base.hpp>
 


enum class OPERATION {ADD, AND, AS, ASSIGN, BAND, BNOT, BOR, BXOR, CMPEQ, CMPGT, CMPGTE, CMPLT, CMPLTE, CMPNE, CONCAT, DECREMENT, DIV, IN, INCREMENT, LNOT, MUL, NEG, NONE, OR, REM, SUB};


class GenericOperator : public ParseTreeNode {
	
	public:
		
		virtual bool isAggregate () {return true;}
		virtual const BasicArray <ParseTreeNode *> & getArgs () const {return args;}
		virtual const nir::Instruction * build (nir::Scope * scope);
		
		GenericOperator (uint32_t line, uint32_t pos) : ParseTreeNode (line, pos) {}
		GenericOperator (const string & iden, std::initializer_list <PTNode> args) : ParseTreeNode ((*args.begin ())->getLine (), (*args.begin ())->getPos ()), iden (iden), args (args) {}
		virtual ~GenericOperator () {for (PTNode n : args) delete n;}
		
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
		
		SpecificOperator (uint32_t line, uint32_t pos) : GenericOperator (line, pos) {}
		SpecificOperator (const string & iden, std::initializer_list <PTNode> args) : GenericOperator (iden, args) {}
		
	protected:
		
		virtual const nir::Instruction * defBuild (nir::Scope * scope);
		
	private:
};
