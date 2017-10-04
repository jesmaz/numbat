#pragma once


#include <ast/node.hpp>
#include <utility/array.hpp>


namespace AST {

class Return : public Node {
	
	public:
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		Return (numbat::lexer::position pos, const numbat::File * file) : Node (pos, file), retVal (nullptr) {}
		Return (numbat::lexer::position pos, const numbat::File * file, const NodePtr & retVal) : Node (pos, file, retVal->getType ()), retVal (retVal) {}
		
	protected:
	private:
		
		NodePtr retVal;
		
};

class Sequence : public Node {
	
	public:
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		const BasicArray <NodePtr> & getNodes () const {return nodes;}
		virtual string toString (text::PrintMode mode) const;
		
		Sequence (numbat::lexer::position pos, const numbat::File * file) : Node (pos, file) {}
		Sequence (numbat::lexer::position pos, const numbat::File * file, const BasicArray <NodePtr> & nodes) : Node (pos, file), nodes (nodes) {}
		Sequence (numbat::lexer::position pos, const numbat::File * file, const TypePtr & type, const BasicArray <NodePtr> & nodes) : Node (pos, file, type), nodes (nodes) {}
		
	protected:
	private:
		
		BasicArray <NodePtr> nodes;
		
};

}
