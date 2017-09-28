#pragma once


#include <ast/operation.hpp>
#include <ast/type.hpp>
#include <forward.hpp>
#include <parse/tree/base.hpp>
#include <utility/report.hpp>


namespace parser {


class GenericOperator : public ParseTreeNode {
	
	public:
		
		virtual bool isAggregate () {return true;}
		virtual const BasicArray <ParseTreeNode *> & getArgs () const {return args;}
		
		GenericOperator (numbat::lexer::position pos) : ParseTreeNode (pos) {}
		GenericOperator (const string & iden, std::initializer_list <PTNode> args) : ParseTreeNode ((*args.begin ())->getPos ()), iden (iden), args (args) {}
		virtual ~GenericOperator () {delAll (args);}
		
	protected:
		
		string iden;
		BasicArray <PTNode> args;
		
	private:
		
		virtual string strDump (text::PrintMode mode);
		
};

template <OPERATION opp>
class SpecificOperator : public GenericOperator {
	
	public:
		
		AST::NodePtr createAST (AST::Context & ctx) {
			
			if (args.empty ()) {
				report::logMessage (report::ERROR, ctx.getSourceFile (), getPos (), "Operator '" + iden + "' has no arguments");
				return nullptr;
				
			} else {
				auto params = args.map <AST::NodePtr> ([&](auto & a){return a->createAST (ctx);});
				return std::make_shared <AST::Unresolved_Operation> (getPos (), ctx.getSourceFile (), iden, params, opp);
				
			}
			
		}
		
		SpecificOperator (numbat::lexer::position pos) : GenericOperator (pos) {}
		SpecificOperator (const string & iden, std::initializer_list <PTNode> args) : GenericOperator (iden, args) {}
		
	protected:
	private:
};


}
