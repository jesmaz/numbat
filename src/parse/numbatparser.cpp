#include "../../include/parse/numbatparser.hpp"


namespace numbat {


NumbatParser::NumbatParser () {
	
	parser.addRules ("BLOCK", {"{}"});
	
	parser.addRules ("E", {"BLOCK", "IDENTIFIER", "LITERAL", "BracketRound", "Range"});
	parser.addRules ("BracketRound", {"()", "(E)"});
	parser.addRules ("BracketSquare", {"[]", "[E]"});
	//parser.addRules ("InitList", {"Init"});
	parser.addRules ("Range", {"[:]", "[:E]", "[E:]", "[E:E]", "[::E]", "[:E:E]", "[E::E]", "[E:E:E]", "[IDENTIFIER in E]"});
	//parser.addRules ("E", {"E I", "val I", "var I"});
	parser.addRules ("MetaTag", {"@IDENTIFIER", "@BracketRound", "@BracketSquare", "MetaTag MetaTag"}, -1, Parser::LTR);
	parser.addRules ("IDENTIFIER", {"IDENTIFIER MetaTag"}, 0, Parser::LTR);
	parser.addRules ("Variable", {"BracketSquare Variable", "Variable BracketSquare", "IDENTIFIER IDENTIFIER", "var IDENTIFIER", "val IDENTIFIER"}, 0, Parser::LTR);
	
	parser.addRules ("E", {"BracketRound->BracketRound"}, 0, Parser::LTR);
	//parser.addRules ("E", {"E->E"}, 1800, Parser::LTR);
	
	
	
	parser.addRules ("E", {"E BracketRound"}, 100, Parser::LTR);
	parser.addRules ("E", {"E BracketSquare"}, 100, Parser::LTR);
	parser.addRules ("E", {"E Range"}, 100, Parser::LTR);
	parser.addRules ("E", {"E.IDENTIFIER", "E.MetaTag"}, 100, Parser::LTR);
	
	parser.addRules ("E", {"E++", "E--"}, 200, Parser::LTR);
	
	parser.addRules ("E", {"++E", "--E", "-E", "!E", "not E", "~E"}, 300, Parser::RTL);
	
	parser.addRules ("E", {"E as E"}, 400, Parser::LTR);
	parser.addRules ("E", {"E in E"}, 450, Parser::LTR);
	
	parser.addRules ("E", {"E*E", "E/E", "E%E"}, 500, Parser::LTR);
	
	parser.addRules ("E", {"E+E", "E-E", "E~E"}, 600, Parser::LTR);
	
	parser.addRules ("E", {"E<<E", "E>>E"}, 700, Parser::LTR);
	
	parser.addRules ("E", {"E&E"}, 800, Parser::LTR);
	
	parser.addRules ("E", {"E^E"}, 900, Parser::LTR);
	
	parser.addRules ("E", {"E|E"}, 1000, Parser::LTR);
	
	parser.addRules ("E", {"E<E", "E<=E", "E>E", "E>=E"}, 1100, Parser::LTR);
	
	parser.addRules ("E", {"E!=E", "E==E"}, 1200, Parser::LTR);
	
	parser.addRules ("E", {"E and E"}, 1300, Parser::LTR);
	
	parser.addRules ("E", {"E or E"}, 1400, Parser::LTR);
	
	parser.addRules ("E", {"Variable:E"}, 1450, Parser::RTL);
	
	parser.addRules ("E", {"E,E"}, 1500, Parser::RTL);
	//parser.addRules ("InitList", {"Init,Init", "Init,InitList", "Init,E"}, 1500, Parser::RTL);
	
	parser.addRules ("E", {"E?E:E", "E=E", "E+=E", "E-=E", "E~=E", "E*=E", "E/=E", "E%=E", "E<<=E", "E>>=E", "E&=E", "E^=E", "E|=E", "E=>E", "Variable=E"}, 1600, Parser::RTL);
	
	parser.addRules ("E", {"if BracketRound E", "if BracketRound E else E", "if MetaTag E", "for Range E", "while BracketRound E"}, 1700, Parser::LTR);
	
	parser.buildRules ();
	
}



};