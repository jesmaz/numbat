#pragma once


#include <ast/node.hpp>
#include <ast/variable.hpp>


namespace AST {


class RawInit : public Node {
	
	public:
		
		const NodePtr & getInit () const {return init;}
		const VarPtr & getVar () const {return var;}
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		string toString (text::PrintMode mode) const;
		
		RawInit (numbat::lexer::position pos, const numbat::File * file, const VarPtr & var, const NodePtr & init) : Node (pos, file, var->getType ()), var (var), init (init) {}
		
	protected:
	private:
		
		VarPtr var;
		NodePtr init;
		
};


}
