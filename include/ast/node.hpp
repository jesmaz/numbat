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
		const numbat::File * getFile () const {return file;}
		
		Node (numbat::lexer::position pos, const numbat::File * file) : pos (pos), file (file) {}
		Node (numbat::lexer::position pos, const numbat::File * file, const TypePtr & type) : pos (pos), file (file), type (type) {}
		
	protected:
	private:
		
		numbat::lexer::position pos;
		const numbat::File * file;
		TypePtr type;
		
};

NodePtr transform (const NodePtr & node);

}
