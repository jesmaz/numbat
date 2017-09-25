#pragma once


#include <ast/passes/abstract.hpp>
#include <forward.hpp>
#include <memory>
#include <utility/text.hpp>


namespace AST {

class Node {
	
	public:
		
		virtual bool isValue () const {return false;}
		virtual void accept (AbstractPass & pass) const=0;
		
		virtual string toString (text::PrintMode mode) const=0;
		
		TypePtr getType () const {return type;}
		
		numbat::lexer::position getPos () const {return pos;}
		
		Node (numbat::lexer::position pos) : pos (pos) {}
		Node (numbat::lexer::position pos, const TypePtr & type) : pos (pos), type (type) {}
		
	protected:
	private:
		
		numbat::lexer::position pos;
		TypePtr type;
		
};

NodePtr transform (const NodePtr & node);

}
