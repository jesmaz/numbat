#ifndef PASERSTACK_HPP
#define PASERSTACK_HPP


#include "../lexer.hpp"
#include "tree/identifier.hpp"
#include "tree/literal.hpp"
#include "tree/operator.hpp"
#include "tree/symbol.hpp"
#include "tree.hpp"

#include <functional>
#include <stack>


namespace numbat {
namespace parser {


struct Stack {
	
	public:
		
		const string & getS () const {return s;}
		
		PTNode accumulate (char rep, int count, int offset, const std::function <PTNode (const std::vector <PTNode> &)> * builder=nullptr);
		PTNode pop ();
		PTNode top () {return nodes.empty () ? nullptr : nodes.top ();}
		
		size_t size () {return nodes.size ();}
		
		void push (char c, const lexer::token & tkn);
		void push (char c, PTNode n) {s.push_back (c); nodes.push (n);}
		void setIdentifier (char i) {identifier = i;}
		void setLiteral (char l) {literal = l;}
		
		Stack () {}
		~Stack () {while (nodes.size ()) delete nodes.top (), nodes.pop ();}
		
	protected:
	private:
		
		Stack (const Stack & s) {}
		const Stack & operator = (const Stack & s) {return *this;}
		
		string s;
		std::stack <PTNode> nodes;
		char literal='L', identifier='I';
		
};


};
};


#endif /*PASERSTACK_HPP*/