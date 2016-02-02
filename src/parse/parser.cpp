#include "../../include/parse/parser.hpp"
//#define DEBUG_PARSER
#ifdef DEBUG_PARSER
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
			
			PARSER_LOG (std::cout << "STATE: " << state.reduce << " '" << state.ptn << "'" << std::endl);
			
			switch (state [c].action) {
				case State::Action::ACCEPT:
					return 0;
				case State::Action::ERROR:
					PARSER_LOG (std::cout << "ERROR: '" << state.ptn << "' unexpected: " << c << " " << stack.getS () << " (" << state.reduce << ")" << std::endl);
					return -1;
				case State::Action::REDUCE:
					return reduce (state, depth)-1;
					break;
				case State::Action::SHIFT: {
					PARSER_LOG (std::cout << "SHIFTING: '" << c << "' (" << int(c) << ")" << std::endl);
					auto n = parseState (state [c].index, depth+1);
					PARSER_LOG (std::cout << "STATE: " << state.reduce << " '" << state.ptn << "'" << std::endl);
					PARSER_LOG (std::cout << "STACK: '" << stack.getS () << "'" << std::endl);
					if (n) return n-1;
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
			while (stack.getS ().back () != programCode and stack.size ()) {
				stack.pop ();
			}
			while (itt != end and itt->type != lexer::TOKEN::semicolon) {
				++itt;
			}
			if (itt != end) ++itt;
			assert (stack.size () <= 1);
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
	
	std::map <string, ProductionRule> prod = generateKernals (), additional, existing;
	
	std::map <string, uint32_t> entries;
	size_t i=0;
	
	{
		
		assert (prod.count (""));
		auto prodRules = prod.find ("")->second.expected;
		assert (prodRules.size () == 1);
		std::map <char, std::map <string, uint32_t>> indexes;
		std::vector <State *> statePool;
		for (auto & e : prodRules) {
			initialState = stateIndex (' ', prod.find ("")->second, seperateKernals (prod), statePool, indexes);
		}
		for (State * s : statePool) {
			states.push_back (*s);
			delete s;
		}
		for (auto & index : indexes) {
			for (auto & i : index.second) {
				string s;
				if (index.first) {
					s = string () + index.first + " : " + i.first;
				} else {
					s = string () + "\\0: " + i.first;
				}
				entries [s] = i.second;
			}
		}
	}
	
#ifdef DEBUG_PARSER
	size_t m=0;
	for (auto & entry : entries) {
		if (not (m%32)) {
			std::cout << "                      ";
			for (size_t i=' '; i<128; ++i) {
				if (charsUsed [i-' ']) {
					std::cout << "  " << char (i) << ' ';
				}
			}
			std::cout << "\n";
		}
		assert (entry.first.size () <= 16);
		std::cout << entry.first;
		for (size_t i=0; i<16-(entry.first.size ()); ++i) {
			std::cout << " ";
		}
		std::cout << text::yel << "0x" << (entry.second > 0xff ? "" : (entry.second > 0xf ? "0" : "00")) << std::hex << entry.second << " ";
		auto & state = states [entry.second];
		size_t k=0;
		for (size_t i=' '; i<128; ++i) {
			//std::cout << " " <<  char (state [i].action);
			if (charsUsed [i-' ']) {
				std::cout << (k%4<=1 ? (k%4==0 ? text::cyn : text::mag) : (k%4==2 ? text::grn : text::blu)) << (state [i].index > 0xff ? "" : (state [i].index > 0xf ? "0" : "00")) << std::hex << state [i].index << char (state [i].action);
				++k;
			}
		}
		std::cout << text::normal << "\n";
		++m;
	}
#endif
	
}

size_t Parser::stateIndex (char reduction, const ProductionRule & rule, const std::map <char, std::map <string, ProductionRule>> & kernals, std::vector <State *> & statePool, std::map <char, std::map <string, uint32_t>> & indexes) {
	
	auto & specificIndex = indexes [reduction];
	if (specificIndex.find (rule.seen) != specificIndex.end ()) {
		return specificIndex [rule.seen];
	}
	
	size_t index = specificIndex [rule.seen] = statePool.size ();
	State * state = new State;
	statePool.push_back (state);
	state->ptn = rule.seen;
	
	
	for (auto & expected : rule.expected) {
		
		auto & exp = expected.first;
		if (exp.empty ()) {
			if (expected.second.rule) state->ptr = expected.second.rule->ptr;
			state->reduce = expected.second.reduce;
			assert (state->reduce);
			state->terminal = expected.second.accept;
			continue;
		}
		
		
		auto & r = (*state) [exp.front ()];
		if (r.action == State::Action::REDUCE) {
			r.action = State::Action::TRYSHIFT;
		} else {
			r.action = State::Action::SHIFT;
		}
		assert (kernals.find (reduction) != kernals.end ());
		assert (kernals.find (reduction)->second.find (rule.seen + exp.front ()) != kernals.find (reduction)->second.end ());
		r.index = stateIndex (reduction, kernals.find (reduction)->second.find (rule.seen + exp.front ())->second, kernals, statePool, indexes);
		
		std::set <char> visited, current, queued = {exp.front ()};
		while (not queued.empty ()) {
			
			current = queued;
			queued.clear ();
			
			for (char c : current) {
				
				if (visited.count (c)) continue;
				visited.insert (c);
				if (rules.find (c) == rules.end ()) continue;
				std::map <char, std::set <string>> expan;
				
				for (auto & rule : rules [c].expantions) {
					
					queued.insert (rule.s.front ());
					expan [rule.s.front ()].insert (rule.s.substr (1));
					
				}
				
				for (auto e : expan) {
					auto & r = (*state) [e.first];
					if (r.action == State::Action::REDUCE) {
						r.action = State::Action::TRYSHIFT;
					} else if (r.action == State::Action::SHIFT) {
						continue;
					} else {
						r.action = State::Action::SHIFT;
					}
					assert (kernals.find (c) != kernals.end ());
					assert (kernals.find (c)->second.find (string () + e.first) != kernals.find (c)->second.end ());
					r.index = stateIndex (c, kernals.find (c)->second.find (string () + e.first)->second, kernals, statePool, indexes);
				}
				
			}
			
		}
		
	}
	
	if (not rule.seen.empty ()) {
		
		for (auto & subRule : rules [rule.seen.back ()].expantions) {
			
			if (rule.seen.back () != subRule.s.front () or subRule.s.size () == 1) continue;
			auto & r = (*state) [subRule.s [1]];
			if (r.action != State::Action::ERROR) continue;
			
			if (not rule.rule or not state->reduce) {
				r.action = State::Action::SHIFT;
				
			} else if (rule.rule->prec == 0) {
				//0 is a special case
				if (subRule.prec < 0) {
					//-1 is also special, it means 'reduce if possible'
					r.action = State::Action::REDUCE;
				} else {
					r.action = State::Action::SHIFT;
				}
				
			} else if (rule.rule->prec > subRule.prec) {
				//Lower precedence
				r.action = State::Action::SHIFT;
				
			} else if (rule.rule->prec < subRule.prec) {
				//Higher precedence
				r.action = State::Action::REDUCE;
				
			} else if (rule.rule->type == LTR) {
				//Left to Right associativity
				r.action = State::Action::REDUCE;
				
			} else if (rule.rule->type == RTL) {
				//Right to Left associativity
				r.action = State::Action::SHIFT;
				
			} else {
				abort ();
			}
			
			char c = rule.seen.back ();
			assert (kernals.find (c) != kernals.end ());
			assert (kernals.find (c)->second.find (subRule.s.substr (0, 2)) != kernals.find (c)->second.end ());
			r.index = stateIndex (c, kernals.find (c)->second.find (subRule.s.substr (0, 2))->second, kernals, statePool, indexes);
			
		}
		
	}
	
	
	if (state->reduce and rule.rule) {
		for (auto & r : state->rules) {
			if (r.action == State::Action::ERROR) {
				r.action = State::Action::REDUCE;
			} else if (r.action == State::Action::SHIFT) {
				r.action = State::Action::TRYSHIFT;
			}
		}
		state->ptr = rule.rule->ptr;
	}
	
	return index;
	
}

std::map <char, std::map <string, Parser::ProductionRule>> Parser::seperateKernals (const std::map <string, ProductionRule> & kernals) const {
	
	std::map <char, std::set <char>> substitutions;
	substitutions [' '].insert (' ');
	substitutions [getCode ("PROGRAM")].insert (' ');
	{
		std::set <std::pair <char, char>> queue;
		std::set <std::pair <char, char>> seen;
		for (auto & r : rules) {
			queue.insert (std::make_pair (r.first, r.first));
		}
		do {
			for (auto q : queue) {
				substitutions [q.first].insert (q.second);
				seen.insert (q);
			}
			queue.clear ();
			for (auto & s : substitutions) {
				for (auto & e : s.second) {
					if (rules.find (e) == rules.end ()) continue;
					for (auto & x : rules.find (e)->second.expantions) {
						auto p = std::make_pair (x.s.front (), s.first);
						if (not seen.count (p)) queue.insert (std::make_pair (x.s.front (), s.first));
					}
				}
			}
		} while (not queue.empty ());
	}
	
	std::map <char, std::map <string, Parser::ProductionRule>> sepK;
	for (auto & k : kernals) {
		for (auto & exp : k.second.expected) {
			for (char c : substitutions [exp.second.reduce]) {
				ProductionRule & pr = sepK [c][k.first];
				assert (k.second.seen == k.first);
				pr.rule = k.second.rule;
				pr.seen = k.second.seen;
				pr.expected.insert (exp);
				assert (exp.second.reduce);
			}
		}
	}
	return sepK;
	
}

std::map <string, Parser::ProductionRule> Parser::generateKernals () const {
	
	std::map <string, ProductionRule> map;
	
	ProductionRule start, stop;
	start.rule = nullptr;
	start.seen = "";
	start.expected [start.seen + getCode ("PROGRAM")] = {nullptr, ' ', false};
	map [""] = start;
	stop.rule = nullptr;
	stop.seen = start.seen + getCode ("PROGRAM");
	stop.expected [""] = {nullptr, ' ', true};
	map [stop.seen] = stop;
	
	for (auto & r : rules) {
		for (auto & e : r.second.expantions) {
			const string & s = e.s;
			for (size_t i=0, l=s.size (); i<l; ++i) {
				const string seen = s.substr (0, i+1);
				ProductionRule & kernal = map [seen];
				kernal.seen = seen;
				kernal.expected [s.substr (i+1)] = {&e, r.first, false};
				assert (r.first);
				if (i+1 == l) {
					assert (not kernal.rule);
					kernal.rule = &e;
				}
			}
		}
	}
	
	return map;
	
}

void Parser::pushJob (PTNode * result, numbat::lexer::tkstring::const_iterator itt, numbat::lexer::tkstring::const_iterator end) const {
	
}


Parser::Parser ()  {
	
	memset (tknIdMap, 0, sizeof (tknIdMap));
	registerRule (lexer::lexToken (";"));
	
}


};