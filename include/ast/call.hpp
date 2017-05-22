#pragma once


#include <ast/function.hpp>
#include <ast/node.hpp>


namespace AST {

class Call_n : public Node {
	
	public:
		
		const FuncPtr & getFunc () const {return func;}
		const BasicArray <NodePtr> & getArgs () const {return args;}
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		Call_n (numbat::lexer::position pos, const FuncPtr & func, const BasicArray <NodePtr> & args) : Node (pos), func (func), args (args) {}
		
	protected:
	private:
		
		FuncPtr func;
		BasicArray <NodePtr> args;
		
};

class Call_0 : public Node {
	
	public:
		
		const FuncPtr & getFunc () const {return func;}
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		Call_0 (numbat::lexer::position pos, const FuncPtr & func) : Node (pos), func (func) {}
		
	protected:
	private:
		
		FuncPtr func;
		
};

class Call_1 : public Node {
	
	public:
		
		const FuncPtr & getFunc () const {return func;}
		const NodePtr & getArg () const {return arg;}
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		Call_1 (numbat::lexer::position pos, const FuncPtr & func, const NodePtr & arg) : Node (pos), func (func), arg (arg) {}
		
	protected:
	private:
		
		FuncPtr func;
		NodePtr arg;
		
};

class Call_2 : public Node {
	
	public:
		
		const FuncPtr & getFunc () const {return func;}
		const NodePtr & getLhs () const {return lhs;}
		const NodePtr & getRhs () const {return rhs;}
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		Call_2 (numbat::lexer::position pos, const FuncPtr & func, const NodePtr & lhs, const NodePtr & rhs) : Node (pos), func (func), lhs (lhs), rhs (rhs) {}
		
	protected:
	private:
		
		FuncPtr func;
		NodePtr lhs, rhs;
		
};

class Unresolved_Call : public Node {
	
	public:
		
		const BasicArray <NodePtr> & getArgs () const {return args;}
		const NodePtr & getCallee () const {return callee;}
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		Unresolved_Call (numbat::lexer::position pos, NodePtr callee, const BasicArray <NodePtr> & args) : Node (pos), callee (callee), args (args) {}
		
	protected:
	private:
		
		NodePtr callee;
		BasicArray <NodePtr> args;
		
};

class Unresolved_Constructor : public Node {
	
	public:
		
		const BasicArray <NodePtr> & getArgs () const {return args;}
		const VarPtr & getVar () const {return var;}
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		Unresolved_Constructor (numbat::lexer::position pos, VarPtr var, const BasicArray <NodePtr> & args) : Node (pos), var (var), args (args) {}
		
	protected:
	private:
		
		VarPtr var;
		BasicArray <NodePtr> args;
		
};

}
