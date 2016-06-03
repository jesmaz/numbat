#include "../../include/parse/numbatparser.hpp"


namespace numbat {


void oppRules (Parser & parser, string rule, std::vector <string> ptns, int16_t prec, numbat::Parser::RuleType type) {
	for (const string & p : ptns) {
		string iden;
		for (char c : p) {
			if (c == ' ') continue;
			if (c == 'E') c = ' ';
			iden.push_back (c);
		}
		parser.addRule (rule, p, prec, type, [=](const std::vector <PTNode> & args) -> PTNode {
			std::vector <PTNode> nargs;
			nargs.reserve (args.size ());
			for (size_t i=0; i<args.size (); ++i) {
				if (args[i]->getType () == ParseTreeNode::NodeType::KEYWORD or args[i]->getType () == ParseTreeNode::NodeType::SYMBOL) {
					delete args[i];
					continue;
				}
				nargs.push_back (args[i]);
			}
			return new ParseTreeOperator (iden, nargs);
		});
	}
}

NumbatParser::NumbatParser () {
	
	auto createCall = [](const std::vector <PTNode> & args) -> PTNode {
		PTNode t;
		if (args.size () == 4) {
			if (args [2]->isList ()) {
				t = new ParseTreeCall (args [0], args [2]->releaseArgs ());
				delete args [2];
			} else {
				t = new ParseTreeCall (args [0], {args [2]});
			}
			delete args [3];
		} else {
			t = new ParseTreeCall (args [0], {});
			delete args [2];
		}
		delete args [1];
		return t;
	};
	
	auto createIndex = [](const std::vector <PTNode> args) -> PTNode {
		
		if (args.size () == 3) {
			delete args [1];
			delete args [2];
			return new ParseTreeIndex (args [0], {});
		}
		assert (args.size () == 4);
		
		delete args [1];
		delete args [3];
		
		if (args[2]->getType () == ParseTreeNode::NodeType::LIST) {
			auto a = args [2]->releaseArgs ();
		
			delete args [2];
			return new ParseTreeIndex (args [0], a);
		} else {
			return new ParseTreeIndex (args [0], {args [2]});
		}
		
	};
	
	auto createList = [](const std::vector <PTNode> args){
		if (args.size () == 1) {
			return new ParseTreeList (args);
		}
		delete args [1];
		if (args[0]->getType () == ParseTreeNode::NodeType::LIST) {
			auto a = args[0]->releaseArgs ();
			a.push_back (args [2]);
			delete args [0];
			return new ParseTreeList (a);
		} else {
			return new ParseTreeList ({args[0],args[2]});
		}
	};
	
	
	auto discardSemicolon = [](const std::vector <PTNode> args) {
		delete args [1];
		return args [0];
	};
	
	
	auto accumulatePair = [](const std::vector <PTNode> args) {
		args [0]->push_back (args [1]);
		return args [0];
	};
	
	auto accumulateBinary = [](const std::vector <PTNode> args) {
		args [0]->push_back (args [2]);
		delete args [1];
		return args [0];
	};
	
	
	parser.addRules ("PROGRAM", {"Statement"}, 0, Parser::LTR, [](const std::vector <PTNode> args){return new ParseTree (args);});
	
	parser.addRules ("PROGRAM", {"PROGRAM Statement"}, 0, Parser::LTR, accumulatePair);
	parser.addRules ("PROGRAM", {"PROGRAM ;"}, 0, Parser::LTR, discardSemicolon);
	
	
	parser.addRules ("Statement", {"Assignment;", "Call;", "Control;", "Variable;", "Parameter;", "VariableList;", "List;", "Import;", "Function;"}, -1, Parser::LTR, discardSemicolon);
	parser.addRules ("Statement", {"Function"}, -1, Parser::LTR);
	
	
	parser.addRules ("Arg", {"E:E"});
	parser.addRules ("ArgList", {"Arg", "ArgList,Arg", "ArgList,E", "List,Arg"}, 1700, Parser::LTR, createList);
	parser.addRules ("ParameterList", {"Parameter", "Parameter,Parameter", "ParameterList,Parameter"}, 1700, Parser::LTR, createList);
	parser.addRules ("VariableList", {"Variable", "VariableList,Arg", "VariableList,Variable"}, 1700, Parser::LTR, createList);
	
	parser.addRules ("Variable", {"Parameter:E"}, 0, Parser::LTR, [](const std::vector <PTNode> args) -> PTNode {
		assert (typeid (*args[0]) == typeid (ParseTreeVariable));
		ParseTreeVariable * var = reinterpret_cast <ParseTreeVariable *> (args[0]);
		assert(not var->releaseInst ());
		PTNode vt = var->releaseVType (), id = var->releaseIden ();
		delete var;
		delete args [1];
		return new ParseTreeVariable (vt, id, args [2]);
	});
	parser.addRules ("Parameter", {"Atom IDENTIFIER", "var IDENTIFIER", "val IDENTIFIER"}, 0, Parser::LTR, [](const std::vector <PTNode> args) -> PTNode {
		auto t = new ParseTreeVariable (args[0], args[1]);
		return t;
	});
	
	parser.addRules ("Assignment", {"List?List:List", "List=List", "List+=List", "List-=List", "List~=List", "List*=List", "List/=List", "List%=List", "List<<=List", "List>>=List", "List&=List", "List^=List", "List|=List", "List=>List"}, 1600, Parser::RTL);
	
	parser.addRules ("List", {"List,E"}, 1500, Parser::LTR, createList);
	parser.addRules ("List", {"Assignment", "E"});
	
	oppRules (parser, "E", {"E or E"}, 1400, Parser::LTR);
	
	oppRules (parser, "E", {"E and E"}, 1300, Parser::LTR);
	
	oppRules (parser, "E", {"E!=E", "E==E"}, 1200, Parser::LTR);
	
	oppRules (parser, "E", {"E<E", "E<=E", "E>E", "E>=E"}, 1100, Parser::LTR);
	
	oppRules (parser, "E", {"E|E"}, 1000, Parser::LTR);
	
	oppRules (parser, "E", {"E^E"}, 900, Parser::LTR);
	
	oppRules (parser, "E", {"E&E"}, 800, Parser::LTR);
	
	oppRules (parser, "E", {"E<<E", "E>>E"}, 700, Parser::LTR);
	
	oppRules (parser, "E", {"E+E", "E-E", "E~E"}, 600, Parser::LTR);
	
	oppRules (parser, "E", {"E*E", "E/E", "E%E"}, 500, Parser::LTR);
	
	oppRules (parser, "E", {"E in E"}, 450, Parser::LTR);
	
	oppRules (parser, "E", {"E as E"}, 400, Parser::LTR);
	
	oppRules (parser, "E", {"++E", "--E", "-E", "!E", "not E", "~E"}, 300, Parser::RTL);
	
	oppRules (parser, "E", {"E++", "E--"}, 200, Parser::LTR);
	
	parser.addRules ("E", {"Atom", "Call"});
	
	parser.addRules ("Atom", {"E.Atom", "E.MetaTag"}, 100, Parser::LTR);
	parser.addRules ("Atom", {"Atom[]", "Atom[E]", "Atom[List]", "E[]", "E[E]", "E[List]"}, 100, Parser::LTR, createIndex);
	parser.addRules ("Call", {"E()", "E(List)", "E(ArgList)"}, 100, Parser::LTR, createCall);
	parser.addRules ("Slice", {"Atom[:]", "Atom[:E]", "Atom[E:]", "Atom[E:E]", "Atom[::E]", "Atom[:E:E]", "Atom[E::E]", "Atom[E:E:E]"}, 100, Parser::LTR, [](const std::vector <PTNode> args) -> PTNode {
		return new ParseTreeSliceDecorator (args [0], new ParseTreeSlice ({args.begin ()+2, args.end ()-1}));
	});
	
	parser.addRules ("Atom", {"Atom MetaTag"});
	parser.addRules ("Atom", {"LITERAL", "IDENTIFIER", "Lambda", "Slice", "Each", "Control", "Block"});
	parser.addRules ("Atom", {"()"}, 0, Parser::LTR, [](const std::vector <PTNode> args) -> PTNode {
		auto l = new ParseTreeList (args [0]->getLine (), args [0]->getPos ()); 
		delete args [0];
		delete args [1];
		return l;
	});
	parser.addRules ("Atom", {"(List)"}, 0, Parser::LTR, [](const std::vector <PTNode> args) -> PTNode {
		delete args [0];
		delete args [2];
		return args [1];
	});
	
	
	parser.addRules ("Block", {"{}", "{PROGRAM}", "{PROGRAM List}", "{List}"});
	parser.addRules ("MetaTag", {"@Atom"});
	
	
	parser.addRules ("Slice", {"[:]", "[:E]", "[E:]", "[E:E]", "[::E]", "[:E:E]", "[E::E]", "[E:E:E]", "[L in L]", "[E in E]"}, 0, Parser::LTR, [](const std::vector <PTNode> args) -> PTNode {
		switch (args.size ()) {
			case 1:
				return args [0];
			default:
				return new ParseTreeSlice ({args.begin ()+1, args.end ()-1});
		}
	});
	
	parser.addRules ("Control", {"if (E) List", "if (E) Block", "if (E) List else List", "if (E) Block else Block", "if MetaTag List", "if MetaTag Block"}, 1700, Parser::LTR, [](const std::vector <PTNode> args){
		delete args [0];
		switch (args.size ()) {
			case 3:
				return new ParseTreeIfElse (args [1], args [2]);
			case 5:
				delete args [1];
				delete args [3];
				return new ParseTreeIfElse (args [2], args [4]);
			case 7:
				delete args [1];
				delete args [3];
				delete args [5];
				return new ParseTreeIfElse (args [2], args [4], args [6]);
			default:
				assert (false);
				break;
		}
	});
	parser.addRules ("Control", {"for Slice List", "for Slice Block"}, 1700, Parser::LTR);
	parser.addRules ("Control", {"while (E) List", "while (E) Block"}, 1700, Parser::LTR);
	
	parser.addRules ("Module", {"import IDENTIFIER", "import LITERAL", "Module.IDENTIFIER", "Module.LITERAL"}, 1800, Parser::LTR, [](const std::vector <PTNode> args){
		switch (args.size ()) {
			case 2:
				delete args [0];
				return new ParseTreeImportPath ({args[1]});
			case 3: {
				auto a = args[0]->releaseArgs ();
				a.push_back (args [2]);
				delete args [0];
				delete args [1];
				return new ParseTreeImportPath (a);
			}
			default:
				return assert ("Incorrect number of arguments" == nullptr), (ParseTreeImportPath*)nullptr;
				break;
		}
	});
	parser.addRules ("Import", {"Module", "Module as IDENTIFIER"}, 1800, Parser::LTR, [](const std::vector <PTNode> args){
		if (args.size () > 2) {
			delete args [1];
			return new ParseTreeImport (args [0], args[2]);
		} else {
			return new ParseTreeImport (args [0]);
		}
	});
	
	parser.addRules ("E", {"()->() Block", "()->(List) Block", "(List)->() Block", "(List)->(List) Block"}, 2000, Parser::LTR);
	parser.addRules ("Lambda", {
		"List->List"
	}, 2000, Parser::LTR, [](const std::vector <PTNode> args){
		delete args [1];
		delete args [2];
		return new Function (nullptr, args [0], args [3], nullptr);
	});
	parser.addRules ("Function", {
		"def IDENTIFIER ()",						//4
		"def IDENTIFIER (ParameterList)",					//5
		"def IDENTIFIER () Statement",				//5
		"def IDENTIFIER (ParameterList) Statement",			//6
		"def IDENTIFIER ()->()",					//8
		"def IDENTIFIER ()->(List)",				//9
		"def IDENTIFIER (ParameterList)->()",				//9
		"def IDENTIFIER (ParameterList)->(List)",			//10
		"def IDENTIFIER ()->() Statement",			//9
		"def IDENTIFIER ()->(List) Statement",		//10
		"def IDENTIFIER (ParameterList)->() Statement",		//10
		"def IDENTIFIER (ParameterList)->(List) Statement",	//11
	}, 2000, Parser::LTR, [](const std::vector <PTNode> args){
		delete args [0];
		delete args [2];
		PTNode iden = args [1], params = nullptr, type = nullptr, body = nullptr;
		size_t pos = 3;
		
		std::cerr << "1: " << args [pos]->toString () << std::endl;
		if (args [pos]->getType () != ParseTreeNode::NodeType::SYMBOL) {
			params = args [pos];
			++pos;
		}
		
		delete args [pos];
		
		if (++pos >= args.size ()) {goto done;}
		
		if (args [pos]->getType () != ParseTreeNode::NodeType::SYMBOL) {
			body = args [pos];
			goto done;
		}
		
		delete args [pos]; ++pos;
		delete args [pos]; ++pos;
		delete args [pos]; ++pos;
		
		if (args [pos]->getType () != ParseTreeNode::NodeType::SYMBOL) {
			type = args [pos];
			++pos;
		}
		
		delete args [pos];
		if (++pos >= args.size ()) {goto done;}
		
		body = args [pos];
		
		done:
		return new Function (iden, params, type, body);
	});
	
	parser.buildRules ();
	
}



};