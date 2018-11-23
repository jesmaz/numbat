#pragma once


#include <ast/node.hpp>
#include <utility/array.hpp>


namespace AST {

class Return : public Node {
	
	public:
		
		const NodePtr & getRetVal () const {return retVal;}
		
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
		const BasicArray <VarPtr> & getLocalStack () const {return localStack;}
		const BasicArray <NodePtr> & getNodes () const {return nodes;}
		virtual string toString (text::PrintMode mode) const;
		
		Sequence (numbat::lexer::position pos, const numbat::File * file) : Node (pos, file) {}
		Sequence (numbat::lexer::position pos, const numbat::File * file, const BasicArray <VarPtr> & localStack, const BasicArray <NodePtr> & nodes) : Node (pos, file), localStack (localStack), nodes (nodes) {}
		Sequence (numbat::lexer::position pos, const numbat::File * file, const TypePtr & type, const BasicArray <VarPtr> & localStack, const BasicArray <NodePtr> & nodes) : Node (pos, file, type), localStack (localStack), nodes (nodes) {}
		
	protected:
	private:
		
		BasicArray <VarPtr> localStack;
		BasicArray <NodePtr> nodes;
		
};

}
