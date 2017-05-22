#pragma once


#include <ast/node.hpp>
#include <utility/array.hpp>


namespace AST {

class Sequence : public Node {
	
	public:
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		const BasicArray <NodePtr> & getNodes () const {return nodes;}
		virtual string toString (text::PrintMode mode) const;
		
		Sequence (numbat::lexer::position pos) : Node (pos) {}
		Sequence (numbat::lexer::position pos, const BasicArray <NodePtr> & nodes) : Node (pos), nodes (nodes) {}
		Sequence (numbat::lexer::position pos, const TypePtr & type, const BasicArray <NodePtr> & nodes) : Node (pos, type), nodes (nodes) {}
		
	protected:
	private:
		
		BasicArray <NodePtr> nodes;
		
};

}
