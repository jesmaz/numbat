#pragma once


#include <ast/node.hpp>
#include <utility/array.hpp>


namespace AST {

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
