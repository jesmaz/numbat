#include "../../include/parse/stack.hpp"

namespace numbat {
namespace parser {


PTNode Stack::accumulate (char rep, int count, int offset, PTNode(*builder)(std::vector <PTNode>)) {
	
	string suffix = s.substr (s.length () - offset);
	s = s.substr (0, s.length () - offset - count + 1);
	s.back () = rep;
	s += suffix;
	std::vector <PTNode> args (count), tmp (offset);
	for (size_t i=offset; i>0;) tmp[--i]=nodes.top (), nodes.pop ();
	while (count) {
		args [--count] = nodes.top (); nodes.pop ();
	}
	PTNode n;
	if (builder) {
		n = builder (args);
	} else if (args.size () == 1) {
		n = args.front ();
	} else {
		n = new ParseTreeOperator (args);
	}
	nodes.push (n);
	for (auto e : tmp) nodes.push (e);
	return n;
	
}

void Stack::push (char c, const lexer::token & tkn) {
	
	s.push_back (c);
	PTNode n;
	if (c == literal) {
		n = new ParseTreeLiteral (tkn.line, 0);
	} else if (c == identifier) {
		n = new ParseTreeIdentifier (tkn.line, 0, tkn.iden);
	} else {
		n = new ParseTreeSymbol (tkn.line, 0, tkn.iden);
	}
	nodes.push (n);
	
}



}
}