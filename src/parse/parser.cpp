#include "../../include/parse/parser.hpp"
#ifdef DEBUG
#define PARSER_LOG(a) a
#else
#define PARSER_LOG(a)
#endif


#include <algorithm>



namespace numbat {


char Parser::getCode (const string & rule) const{
	auto itt = idmap.find (rule);
	if (itt==idmap.end ()) return 0;
	else return itt->second;
}

char Parser::getCode (lexer::TOKEN token) const {
	return tknIdMap [int (token)];
}

char Parser::getCode (lexer::token tkn) const {
	char c =0;
	switch (tkn.type) {
		case lexer::TOKEN::symbol:
			c = getCode (tkn.iden);
			break;
		case lexer::TOKEN::whitespace:
		case lexer::TOKEN::indent:
			break;
		case lexer::TOKEN::numericliteral:
		case lexer::TOKEN::chararrayliteral:
		case lexer::TOKEN::stringliteral:
			c = getCode ("LITERAL");
			break;
		default:
			c = getCode (tkn.type);
			break;
	}
	return c;
}

template <typename K, typename M>
char tregisterCode (const K & key, char c, M & map, std::bitset <128 - ' '> & charsUsed, char & lastChar) {
	if (c) {
		int index = c-' ';
		charsUsed [index] = true;
		return map [key] = c;
	} else if (char r = map [key]) {
		return r;
	} else {
		int index = lastChar-' ';
		while (charsUsed[index]) ++index, ++lastChar;
		charsUsed [index] = true;
		return map [key] = lastChar++;
	}
}

char Parser::registerCode (const lexer::token & tkn, char c) {
	return registerCode (tkn.type, c);
}

char Parser::registerCode (lexer::TOKEN type, char c) {
	return tregisterCode (int (type), c, tknIdMap, charsUsed, lastChar);
}

char Parser::registerCode (const string & str, char c) {
	return tregisterCode (str, c, idmap, charsUsed, lastChar);
}

char Parser::registerRule (const lexer::token & tkn) {
	const string s = tkn.iden;
	lexer::TOKEN type = tkn.type;
	char done = type == lexer::TOKEN::identifier ? getCode (tkn.iden) : getCode (tkn);
	if (done) return done;
	for (char c : s) {
		if (not charsUsed [int (c-' ')]) {
			done = registerCode (s, c);
			break;
		}
	}
	if (not done) done = registerCode (s);
	
	if (type != lexer::TOKEN::identifier or s == "IDENTIFIER") {
		registerCode (type, done);
	} else if (s == "LITERAL") {
		registerCode (lexer::TOKEN::chararrayliteral, done);
		registerCode (lexer::TOKEN::numericliteral, done);
		registerCode (lexer::TOKEN::stringliteral, done);
	}
	PARSER_LOG(std::cout << "'" << done << "' : '" << s << "'" << std::endl;)
	return done;
}

PTNode Parser::parse (const numbat::lexer::tkstring & prog) {
	
	if (states.empty ()) buildRules ();
	
	auto itt = prog.begin ();
	auto end = prog.end ();
	
	return parse (itt, end);
	
}

PTNode Parser::parse (numbat::lexer::tkstring::const_iterator start, numbat::lexer::tkstring::const_iterator end) const {
	
	std::stack <const State *> stateStack;
	parser::Stack stack;
	stack.setIdentifier (getCode ("IDENTIFIER"));
	stack.setLiteral (getCode ("LITERAL"));
	const char programCode = getCode ("PROGRAM");
	auto itt = start, tkn = start;
	char c;
	const auto next = [&]() {
		while (itt != end and (itt->type == lexer::TOKEN::whitespace or itt->type == lexer::TOKEN::indent)) ++itt;
		if (itt==end) return false;
		c = getCode (*itt);
		tkn = itt;
		if (c) {
			stack.push (c, *tkn);
			PARSER_LOG (std::cout << "PEAKING: '" << c << "' (" << int(c) << ")(" << tkn->iden << ")" << std::endl);
		}
		itt++;
		return true;
	};
	
	const std::function <ssize_t(const State & state, size_t)> reduce = [&](const State & state, size_t depth) {
		PARSER_LOG (std::cout << "REDUCING '" << state.ptn << "' to '"<< state.reduce << "' in '" << stack.getS () << "' NEXT: '" << c << "'" << std::endl);
		size_t pos = stack.getS ().rfind (state.ptn, depth-2);
		PARSER_LOG (std::cout << "         " << stack.getS ().size () << " " << depth << " " << pos << std::endl);
		if (pos == string::npos) {
			std::cerr << "Fatal error: No match for selected rule '" << state.ptn << "' in '" << stack.getS () << "'" << std::endl; exit (1);
		}
		assert (state.reduce);
		stack.accumulate (state.reduce, state.ptn.size (), pos, state.ptr);
		c = state.reduce;
		return state.ptn.size ();
	};
	
	assert (not states.empty ());
	
	const std::function <ssize_t(size_t, size_t)> parseState = [&](size_t index, size_t depth) -> ssize_t {
		
		const State & state = states [index];
		if (depth > stack.size ()) {
			next ();
		} else {
			c = stack.getS ().back ();
		}
		
		while (true) {
			
			PARSER_LOG (std::cout << "STATE: " << state.reduce << " '" << state.ptn << "' '" << c << "' 0x" << std::hex << index << std::dec << std::endl);
			
			switch (state [c].action) {
				case State::Action::ACCEPT:
					return 0;
				case State::Action::CHANGE_STATE: {
					parseState (state [c].index, depth);
					break;
				}
				case State::Action::ERROR:
					PARSER_LOG (std::cout << "ERROR: '" << state.ptn << "' unexpected: " << c << " " << stack.getS () << " (" << state.reduce << ")" << std::endl);
					return -1;
				case State::Action::REDUCE:
					return reduce (state, depth)-1;
					break;
				case State::Action::SHIFT: {
					PARSER_LOG (std::cout << "SHIFTING: '" << c << "' (" << int(c) << ")" << std::endl);
					auto n = parseState (state [c].index, depth+1);
					PARSER_LOG (std::cout << "STATE: " << state.reduce << " '" << state.ptn << "' '" << c << "' 0x" << std::hex << index << std::dec << std::endl);
					PARSER_LOG (std::cout << "STACK: '" << stack.getS () << "'" << std::endl);
					if (n) return n-1;
					return 0;
					break;
				}
				case State::Action::TRYSHIFT: {
					PARSER_LOG (std::cout << "SHIFTING: '" << c << "' (" << int(c) << ")" << std::endl);
					size_t oldStack = stack.size ();
					auto olditt = itt;
					auto n = parseState (state [c].index, depth+1);
					if (n < 0) {
						for (size_t i=0, l=stack.size ()-oldStack; i<l; ++i) {
							stack.pop ();
						}
						itt = olditt;
						PARSER_LOG (std::cout << "STATE: " << state.reduce << " '" << state.ptn << "'" << std::endl);
						PARSER_LOG (std::cout << "STACK: '" << stack.getS () << "'" << std::endl);
						return reduce (state, depth)-1;
					} else if (n) return n-1;
					break;
				}
			}
		}
		return 0;
	};
	
	while (true) {
		parseState (initialState, 1);
		if (stack.size () > 1) {
			while (stack.size () > 1) {
				stack.pop ();
			}
			if (stack.getS ().back () != programCode) stack.pop ();
			while (itt != end and itt->type != lexer::TOKEN::semicolon) {
				++itt;
			}
			if (itt != end) ++itt;
		} else {
			break;
		}
	}
	
	PTNode node;
	if (stack.size () != 1) {
		PTNode n = stack.top ();
		if (n) {
			node = new ParseTreeError (n->getLine (), n->getPos (), "Unexpected end of expression");
		} else {
			node = new ParseTreeError (0, 0, "Expected expression");
		}
		std::cerr << "'" << stack.getS () << "'" << std::endl;
		std::cerr << stack.size () << std::endl;
	} else {
		node = stack.pop ();
	}
	return node;
	
}


void Parser::addRule (const string & rule, const string & ptn, int16_t prec, RuleType ruleType, std::function <PTNode (const std::vector <PTNode> &)> ptr) {
	
	auto tks = lexer::lex (ptn);
	char rc = registerRule (lexer::lexToken (rule));
	string pattern;
	
	for (auto & tkn : tks) {
		switch (tkn.type) {
			case lexer::TOKEN::whitespace:
			case lexer::TOKEN::indent:
				break;
			case lexer::TOKEN::semicolon:
				if (tkn.iden != ";") break;
			default:
				pattern += registerRule (tkn);
				break;
		}
	}
	
	auto builder = ptr ? new std::function <PTNode (const std::vector <PTNode> &)> (ptr) : nullptr;
	rules [rc].expantions.push_back ({pattern, builder, prec, ruleType});
	
}

void Parser::addRules (const string & rule, const std::vector <string> & ptn, int16_t prec, int ruleType, std::function <PTNode (const std::vector <PTNode> &)> ptr) {
	for (const string & s : ptn) {
		addRule (rule, s, prec, RuleType (ruleType), ptr);
	}
}

void Parser::buildRules () {
	
	std::map <char, std::set <char>> synonyms, oneToOne;
	std::map <string, std::set <char>> reductions, shifts;
	std::map <string, Rule::Args> ruleMap;
	std::map <string, int32_t> stateIndex;
	std::map <string, char> uniqeReductions;
	
	{
		for (auto & e : rules) {
			const Rule & r = e.second;
			std::set <char> & set = synonyms [e.first];
			for (const Rule::Args & arg : r.expantions) {
				set.insert (arg.s.front ());
				for (uint64_t i=1, l=arg.s.length (); i<l; ++i) {
					shifts [arg.s.substr (0, i)].insert (e.first);
				}
				reductions [arg.s].insert (e.first);
				if (arg.s.length () == 1) {
					oneToOne [arg.s.front ()].insert (e.first);
					oneToOne [arg.s.front ()].insert (arg.s.front ());
				}
			}
		}
		bool cont = true;
		while (cont) {
			cont = false;
			for (auto & e : synonyms) {
				std::set <char> set = e.second;
				for (char c : set) {
					e.second.insert (synonyms [c].begin (), synonyms [c].end ());
				}
				cont |= set.size () != e.second.size ();
			}
		}
		cont = true;
		while (cont) {
			cont = false;
			for (auto & e : oneToOne) {
				std::set <char> set = e.second;
				for (char c : set) {
					e.second.insert (oneToOne [c].begin (), oneToOne [c].end ());
				}
				cont |= set.size () != e.second.size ();
			}
		}
		for (auto & e : rules) {
			for (const Rule::Args & arg : e.second.expantions) {
				for (char c : oneToOne [e.first]) {
					if (ruleMap.count (c + (" -> " + arg.s))) abort ();
					ruleMap [c + (" -> " + arg.s)] = arg;
					uniqeReductions [c + (" -> " + arg.s)] = e.first;
				}
				ruleMap [e.first + (" -> " + arg.s)] = arg;
				uniqeReductions [e.first + (" -> " + arg.s)] = e.first;
			}
		}
	}
	
	for (auto rule : ruleMap) {
		//std::cout << "'" << rule.first << "'\n";
	}
	//std::cout << "\n\n\n";
	
	std::function <int32_t (const std::set <char>&, const string &)> generateState = [&](const std::set <char> & targets, const string & seen)->int32_t {
		
		assert (not targets.empty ());
		
		string target;
		for (char t : targets) {
			target += t + string(" ");
		}
		
		std::string key = target + std::string ("-> ") + seen;
		
		auto siItt = stateIndex.find (key);
		if (siItt != stateIndex.end ()) {
			return siItt->second;
		}
		
		auto index = stateIndex [key] = this->states.size ();
		State s;
		states.resize (index+1);
		s.ptn = seen;
		
		//std::cout << "Searching: '" << key << "'\n";
		std::map <char, std::set <char>> expected;
		std::list <std::pair <char, const Rule::Args *>> reductions;
		
		State::Action def = State::Action::ERROR;
		
		for (char t : targets) {
			
			std::string key = t + std::string (" -> ") + seen;
			
			for (std::map <string, Rule::Args>::iterator itt=ruleMap.lower_bound (key), end=ruleMap.end (); itt!=end and itt->first.find (key) == 0; ++itt) {
				//std::cout << '\t' << itt->first << "\n";
			}
			
			for (std::map <string, Rule::Args>::iterator itt=ruleMap.lower_bound (key), end=ruleMap.end (); itt!=end and itt->first.find (key) == 0; ++itt) {
				
				//std::cout << itt->first << "\n";
				int diff = itt->first.length () - key.length ();
				if (diff > 0) {
					
					char orig = itt->first [key.length ()];
					
					for (char c : synonyms [orig]) {
						if (c != orig) {
							expected [c].insert (orig);
						}
					}
					expected [orig].insert (t);
					
				} else if (not diff) {
					
					reductions.push_back (std::make_pair (t, &itt->second));
					
				}
				
			}
		
		}
		
		for (auto & exp : expected) {
			
			State::R & r = s [exp.first];
			
			r.action = State::Action::SHIFT;
			
			if (exp.second.count (exp.first)) {
				r.index = generateState (exp.second, seen + exp.first);
			} else {
				r.index = generateState (exp.second, string ("") + exp.first);
			}
			
		}
		
		for (int c=' '; c<128; ++c) {
			
			if (s [c].action == State::Action::ERROR) {
				s [c].action = def;
			}
			
		}
		
		states [index] = s;
		return index;
		 
	};
	
	
	
	initialState = generateState ({getCode ("PROGRAM")}, "");
	
#ifdef DEBUG
	for (auto & entry : stateIndex) {
		assert (entry.first.size () <= 32);
		std::cout << entry.first;
		for (size_t i=0; i<32-(entry.first.size ()); ++i) {
			std::cout << " ";
		}
		std::cout << text::yel << "0x" << (entry.second > 0xff ? "" : (entry.second > 0xf ? "0" : "00")) << std::hex << entry.second << " ";
		auto & state = states [entry.second];
		size_t k=0;
		for (size_t i=' '; i<128; ++i) {
			if (charsUsed [i-' ']) {
				std::cout << (k%4<=1 ? (k%4==0 ? text::cyn : text::mag) : (k%4==2 ? text::grn : text::blu)) << (state [i].index > 0xff ? "" : (state [i].index > 0xf ? "0" : "00")) << std::hex << state [i].index;
				++k;
			}
		}
		k=0;
		std::cout << text::normal << "\n                                      ";
		for (size_t i=' '; i<128; ++i) {
			if (charsUsed [i-' ']) {
				std::cout << (k%4<=1 ? (k%4==0 ? text::cyn : text::mag) : (k%4==2 ? text::grn : text::blu)) << char (i) << ' ' << char (state [i].action);
				++k;
			}
		}
		std::cout << text::normal << "\n";
	}
#endif
	
}

std::map <string, Parser::ProductionRule> Parser::generateProductionRules () const {
	
	std::map <string, ProductionRule> pRules;
	
}

void Parser::pushJob (PTNode * result, numbat::lexer::tkstring::const_iterator itt, numbat::lexer::tkstring::const_iterator end) const {
	
}


Parser::Parser ()  {
	
	memset (tknIdMap, 0, sizeof (tknIdMap));
	registerRule (lexer::lexToken (";"));
	
}


};