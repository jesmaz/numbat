#pragma once


#include <ast/function.hpp>


namespace AST {


class Operation : public Node {
	
	public:
		
		Operation (numbat::lexer::position pos) : Node (pos) {}
		Operation (numbat::lexer::position pos, const TypePtr & type) : Node (pos, type) {}
		
	protected:
	private:
		
		
		
};

class Unresolved_Operation : public Operation {
	
	public:
		
		const BasicArray <NodePtr> & getArgs () const {return args;}
		const parser::OPERATION & getOpp () const {return opp;}
		const string & getIden () const {return iden;}
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		
		virtual string toString (text::PrintMode mode) const {
			string s = iden + "[unresolved] (";
			if (not args.empty ()) {
				s += args.front ()->toString (mode);
				for (size_t i=1; i<args.size (); ++i) {
					s += ", " + args [i]->toString (mode);
				}
			}
			return s + ")";
		}
		
		Unresolved_Operation (numbat::lexer::position pos, const string & iden, const BasicArray <NodePtr> & args, parser::OPERATION opp) : 
		                      Operation (pos),
		                      iden (iden),
		                      args (args),
		                      opp (opp) {}
		
	protected:
	private:
		
		string iden;
		BasicArray <NodePtr> args;
		parser::OPERATION opp;
		
};

class Basic_Operation : public Operation {
	
	public:
		
		const BasicArray <NodePtr> & getArgs () const {return args;}
		const string & getIden () const {return iden;}
		const parser::OPERATION getOpp () const {return opp;}
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		
		virtual string toString (text::PrintMode mode) const {
			string s = iden + "(";
			if (not args.empty ()) {
				s += args.front ()->toString (mode);
				for (size_t i=1; i<args.size (); ++i) {
					s += ", " + args [i]->toString (mode);
				}
			}
			return s + ")";
		}
		
		Basic_Operation (numbat::lexer::position pos, const string & iden, const BasicArray <NodePtr> & args, parser::OPERATION opp) : 
		                 Operation (pos),
		                 iden (iden),
		                 args (args),
		                 opp (opp)  {}
		
		Basic_Operation (numbat::lexer::position pos, const TypePtr & type, const string & iden, const BasicArray <NodePtr> & args, parser::OPERATION opp) : 
		                 Operation (pos, type),
		                 iden (iden),
		                 args (args),
		                 opp (opp)  {}
		
	protected:
	private:
		
		string iden;
		BasicArray <NodePtr> args;
		parser::OPERATION opp;
		
};


}
