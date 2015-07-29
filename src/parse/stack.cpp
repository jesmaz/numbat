#include "../../include/parse/stack.hpp"

namespace numbat {
namespace parser {


PTNode Stack::accumulate (char rep, int count, int offset, const std::function <PTNode (const std::vector <PTNode> &)> * builder) {
	
	string suffix = s.substr (offset + count);
	s = s.substr (0, offset + 1);
	s.back () = rep;
	s += suffix;
	std::vector <PTNode> args (count), tmp (suffix.size ());
	for (size_t i=tmp.size (); i>0;) tmp[--i]=nodes.top (), nodes.pop ();
	while (count) {
		args [--count] = nodes.top (); nodes.pop ();
	}
	PTNode n;
	if (builder) {
		n = (*builder) (args);
	} else if (args.size () == 1) {
		n = args.front ();
	} else {
		n = new ParseTreeOperator ("", args);
	}
	nodes.push (n);
	for (auto e : tmp) nodes.push (e);
	return n;
	
}

PTNode Stack::pop () {
	
	if (nodes.empty ()) return nullptr;
	PTNode node = nodes.top ();
	nodes.pop ();
	s.pop_back ();
	return node;
	
}

void Stack::push (char c, const lexer::token & tkn) {
	
	s.push_back (c);
	PTNode n;
	if (c == literal) {
		n = new ParseTreeLiteral (tkn.line, 0, tkn.iden, tkn.type);
	} else if (c == identifier) {
		n = new ParseTreeIdentifier (tkn.line, 0, tkn.iden);
	} else {
		if (tkn.iden.length () > 1) {
			n = new ParseTreeKeyword (tkn.line, 0, tkn.iden);
		} else {
			n = new ParseTreeSymbol (tkn.line, 0, tkn.iden);
		}
	}
	nodes.push (n);
	
}



}
}