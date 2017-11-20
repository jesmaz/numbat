#pragma once


#include <ast/node.hpp>
#include <ast/variable.hpp>


namespace AST {


class RawInit : public Node {
	
	public:
		
		const BasicArray <NodePtr> & getArgs () const {return args;}
		const VarPtr & getVar () const {return var;}
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		string toString (text::PrintMode mode) const;
		
		RawInit (numbat::lexer::position pos, const numbat::File * file, const VarPtr & var, const BasicArray <NodePtr> & args) : Node (pos, file, var->getType ()), var (var), args (args) {}
		
	protected:
	private:
		
		VarPtr var;
		BasicArray <NodePtr> args;
		
};


}
