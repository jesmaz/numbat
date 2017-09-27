#pragma once


#include <ast/node.hpp>


namespace AST {


class And : public Node {
	
	public:
		
		const NodePtr & getFirst () const {return first;}
		const NodePtr & getSecond () const {return second;}
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		And (numbat::lexer::position pos, const numbat::File * file, const NodePtr & first, const NodePtr & second) : Node (pos, file), first (first), second (second) {}
		
	protected:
	private:
		
		NodePtr first, second;
		
};

class IfElse : public Node {
	
	public:
		
		const NodePtr & getAlt () const {return alt;}
		const NodePtr & getBody () const {return body;}
		const NodePtr & getCond () const {return cond;}
		const VarPtr & getVar () const {return retVar;}
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		IfElse (numbat::lexer::position pos, const numbat::File * file, const NodePtr & cond, const NodePtr & body, const NodePtr & alt) : Node (pos, file), cond (cond), body (body), alt (alt) {}
		IfElse (numbat::lexer::position pos, const numbat::File * file, const TypePtr & type, const NodePtr & cond, const NodePtr & body, const NodePtr & alt) : Node (pos, file, type), cond (cond), body (body), alt (alt) {}
		IfElse (numbat::lexer::position pos, const numbat::File * file, const VarPtr & var, const NodePtr & cond, const NodePtr & body, const NodePtr & alt);
		
	protected:
	private:
		
		NodePtr cond, body, alt;
		VarPtr retVar;
		
};

class Loop : public Node {
	
	public:
		
		//void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		Loop (numbat::lexer::position pos, const numbat::File * file, const NodePtr & init, const NodePtr & cond, const NodePtr & step, const NodePtr & body, const VarPtr & array) : Node (pos, file), init (init), cond (cond), step (step), body (body), array (array) {}
		
	protected:
	private:
		
		NodePtr init, cond, step, body;
		VarPtr array;
		
};

class Or : public Node {
	
	public:
		
		const NodePtr & getFirst () const {return first;}
		const NodePtr & getSecond () const {return second;}
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		Or (numbat::lexer::position pos, const numbat::File * file, const NodePtr & first, const NodePtr & second) : Node (pos, file), first (first), second (second) {}
		
	protected:
	private:
		
		NodePtr first, second;
		
};

class Unresolved_IfElse : public Node {
	
	public:
		
		const NodePtr & getAlt () const {return alt;}
		const NodePtr & getBody () const {return body;}
		const NodePtr & getCond () const {return cond;}
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		Unresolved_IfElse (numbat::lexer::position pos, const numbat::File * file, const NodePtr & cond, const NodePtr & body, const NodePtr & alt) : Node (pos, file), cond (cond), body (body), alt (alt) {}
		Unresolved_IfElse (numbat::lexer::position pos, const numbat::File * file, const TypePtr & type, const NodePtr & cond, const NodePtr & body, const NodePtr & alt) : Node (pos, file, type), cond (cond), body (body), alt (alt) {}
		
	protected:
	private:
		
		NodePtr cond, body, alt;
		
};

class Unresolved_Loop : public Node {
	
	public:
		
		//void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		Unresolved_Loop (numbat::lexer::position pos, const numbat::File * file, const NodePtr & init, const NodePtr & cond, const NodePtr & step, const NodePtr & body) : Node (pos, file), init (init), cond (cond), step (step), body (body) {}
		
	protected:
	private:
		
		NodePtr init, cond, step, body;
		
};


}
