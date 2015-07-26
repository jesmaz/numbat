#include "../../include/parse/numbatparser.hpp"


namespace numbat {


NumbatParser::NumbatParser () {
	
	parser.addRules ("BLOCK", {"{}"});
	
	parser.addRules ("E", {"BLOCK", "IDENTIFIER", "LITERAL", "Lambda", "BracketRound", "Slice", "Each"});
	parser.addRules ("BracketRound", {"()", "(E)"}, 0, Parser::NONE, [](const std::vector <PTNode> & args) -> PTNode {
		size_t l=args.front ()->getLine (), p=args.front ()->getPos ();
		delete args.front ();
		delete args.back ();
		switch (args.size ()) {
			case 2:
				return new ParseTreeList (l, p);
			case 3:
				if (args [1]->getType () == ParseTreeNode::NodeType::LIST) {
					return args [1];
				}
			default:
				return new ParseTreeList ({args.begin ()+1, args.end ()-1});
		}
	});
	parser.addRules ("BracketSquare", {"[]", "[E]"});
	//parser.addRules ("InitList", {"Init"});
	parser.addRules ("Slice", {"[:]", "[:E]", "[E:]", "[E:E]", "[::E]", "[:E:E]", "[E::E]", "[E:E:E]", "[Each]"});
	//parser.addRules ("E", {"E I", "val I", "var I"});
	parser.addRules ("MetaTag", {"@IDENTIFIER", "@BracketRound", "@BracketSquare"}, -1, Parser::LTR);
	parser.addRules ("IDENTIFIER", {"IDENTIFIER MetaTag"}, 0, Parser::LTR);
	parser.addRules ("Variable", {"Variable BracketSquare", "IDENTIFIER IDENTIFIER", "var IDENTIFIER", "val IDENTIFIER", "const Variable", "ref Variable"}, 0, Parser::LTR);
	
	parser.addRules ("Lambda", {"BracketRound->BracketRound", "BracketRound->BracketRound E"}, 0, Parser::LTR);
	parser.addRules ("Function", {"def IDENTIFIER Lambda", "def IDENTIFIER BracketRound E"}, 0, Parser::LTR, [](const std::vector <PTNode> args){return new Function (args);});
	parser.addRules ("Struct", {"struct IDENTIFIER BLOCK"}, 0, Parser::LTR, [](const std::vector <PTNode> args){return new Struct (args);});
	parser.addRules ("T", {"extern Function", "Function"});
	//parser.addRules ("E", {"E->E"}, 1800, Parser::LTR);
	
	
	
	parser.addRules ("E", {"E BracketRound"}, 100, Parser::LTR);
	parser.addRules ("E", {"E BracketSquare"}, 100, Parser::LTR);
	parser.addRules ("Slice", {"E Slice"}, 100, Parser::LTR);
	parser.addRules ("E", {"E.IDENTIFIER", "E.MetaTag"}, 100, Parser::LTR);
	
	parser.addRules ("E", {"E++", "E--"}, 200, Parser::LTR);
	
	parser.addRules ("E", {"++E", "--E", "-E", "!E", "not E", "~E"}, 300, Parser::RTL);
	
	parser.addRules ("E", {"E as E"}, 400, Parser::LTR);
	parser.addRules ("E", {"E in E"}, 450, Parser::LTR);
	parser.addRules ("Each", {"IDENTIFIER in E"}, 450, Parser::LTR);
	
	parser.addRules ("E", {"E*E", "E/E", "E%E"}, 500, Parser::LTR);
	
	parser.addRules ("E", {"E+E", "E-E", "E~E"}, 600, Parser::LTR);
	
	parser.addRules ("E", {"E<<E", "E>>E"}, 700, Parser::LTR);
	
	parser.addRules ("E", {"E&E"}, 800, Parser::LTR);
	parser.addRules ("Slice", {"Slice&Slice"}, 800, Parser::LTR);
	
	parser.addRules ("E", {"E^E"}, 900, Parser::LTR);
	parser.addRules ("Slice", {"Slice^Slice"}, 900, Parser::LTR);
	
	parser.addRules ("E", {"E|E"}, 1000, Parser::LTR);
	parser.addRules ("Slice", {"Slice|Slice"}, 1000, Parser::LTR);
	
	parser.addRules ("E", {"E<E", "E<=E", "E>E", "E>=E"}, 1100, Parser::LTR);
	
	parser.addRules ("E", {"E!=E", "E==E"}, 1200, Parser::LTR);
	
	parser.addRules ("E", {"E and E"}, 1300, Parser::LTR);
	
	parser.addRules ("E", {"E or E"}, 1400, Parser::LTR);
	
	parser.addRules ("E", {"Variable:E"}, 1450, Parser::RTL);
	
	parser.addRules ("E", {"E,E"}, 1500, Parser::RTL);
	//parser.addRules ("InitList", {"Init,Init", "Init,InitList", "Init,E"}, 1500, Parser::RTL);
	
	parser.addRules ("E", {"E?E:E", "E=E", "E+=E", "E-=E", "E~=E", "E*=E", "E/=E", "E%=E", "E<<=E", "E>>=E", "E&=E", "E^=E", "E|=E", "E=>E", "Variable=E"}, 1600, Parser::RTL);
	
	parser.addRules ("E", {"if BracketRound E", "if BracketRound E else E", "if MetaTag E", "for BracketSquare E", "for Slice E", "while BracketRound E"}, 1700, Parser::LTR);
	
	parser.addRules ("Import", {"import IDENTIFIER", "import LITERAL", "Import.IDENTIFIER", "Import.LITERAL"}, 1800, Parser::LTR);
	parser.addRules ("T", {"Import", "Import as IDENTIFIER"}, 1800, Parser::LTR);
	
	parser.buildRules ();
	
}



};