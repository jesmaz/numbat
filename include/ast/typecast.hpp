#pragma once


#include <ast/node.hpp>


namespace AST {


class CastToNumber : public Node {
	
	public:
		
		const NodePtr & getNode () const {return node;}
		
		virtual string toString (text::PrintMode mode) const;
		
		CastToNumber (numbat::lexer::position pos, const numbat::File * file, const TypePtr & type, const NodePtr & node) : Node (pos, file, type), node (node) {}
		
	protected:
	private:
		
		NodePtr node;
		
};

class CastToArbPrec : public CastToNumber {
	
	public:
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		
		CastToArbPrec (numbat::lexer::position pos, const numbat::File * file, const TypePtr & type, const NodePtr & node) : CastToNumber (pos, file, type, node) {}
		
	protected:
	private:
		
};

class CastToInt : public CastToNumber {
	
	public:
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		
		CastToInt (numbat::lexer::position pos, const numbat::File * file, const TypePtr & type, const NodePtr & node) : CastToNumber (pos, file, type, node) {}
		
	protected:
	private:
		
};

class CastToDecimal : public CastToNumber {
	
	public:
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		
		CastToDecimal (numbat::lexer::position pos, const numbat::File * file, const TypePtr & type, const NodePtr & node) : CastToNumber (pos, file, type, node) {}
		
	protected:
	private:
		
};

class CastToFloat : public CastToNumber {
	
	public:
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		
		CastToFloat (numbat::lexer::position pos, const numbat::File * file, const TypePtr & type, const NodePtr & node) : CastToNumber (pos, file, type, node) {}
		
	protected:
	private:
		
};

class CastToUint : public CastToNumber {
	
	public:
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		
		CastToUint (numbat::lexer::position pos, const numbat::File * file, const TypePtr & type, const NodePtr & node) : CastToNumber (pos, file, type, node) {}
		
	protected:
	private:
		
};


}
