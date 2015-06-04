#include "../../include/parse/parser.hpp"

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
		case lexer::TOKEN::semicolon:
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
	return done;
}

PTNode Parser::parse (const numbat::lexer::tkstring & prog) {
	
	if (!root) buildRules ();
	
	auto itt = prog.begin ();
	auto end = prog.end ();
	
	return parseBody (itt, end);
	
}

PTNode Parser::parseBody (numbat::lexer::tkstring::const_iterator itt, numbat::lexer::tkstring::const_iterator end) const {
	
	auto last = itt;
	std::list <PTNode> list;
	auto next= itt + 1;
	int brace = 0;
	auto shift = [&] () {
		itt = next;
		do {
			if (next != end) ++next;
		} while (next != end and (next->type == lexer::TOKEN::whitespace or next->type == lexer::TOKEN::indent));
		if (itt->type == lexer::TOKEN::symbol) {
			if (itt->iden [0] == '{') ++brace;
			else if (itt->iden [0] == '}') --brace;
		}
		return itt;
	};
	
	auto reduce = [&] () {
		PTNode node = parseExpr (last, itt);
		if (node) {
			list.push_back (node);
#ifdef DEBUG
			std::cout << (node->toString ()) << std::endl;
#endif
		}
		
		return last = next;
	};
	
	while (itt != end and itt->type != lexer::TOKEN::eof) {
		if (!brace and itt->type == lexer::TOKEN::semicolon and (itt->iden == ";" or (next->type != lexer::TOKEN::elsetkn and next->iden != "{")) and itt != last) {
			reduce ();
			shift ();
		} else {
			shift ();
		}
	}
	
	reduce ();
	ParseTree * pt;
	if (list.empty () or not list.front ()) {
		pt = new ParseTree (0, 0);
	} else {
		pt = new ParseTree ({list.begin (), list.end ()});
	}
	return pt;
	
}

PTNode Parser::parseExpr (numbat::lexer::tkstring::const_iterator itt, numbat::lexer::tkstring::const_iterator end) const {
	
	parser::Stack stack;
	char next=getCode (*itt);
	
	auto reduce = [&] (const string & rule, char c) {
		size_t pos = stack.getS ().find_last_of (rule);
		if (!c) abort ();
		if (pos == string::npos) {
			std::cerr << "Fatal error: No match for selected rule '" << rule << "' in '" << stack.getS () << "'" << std::endl; exit (1);
		}
		stack.accumulate (c, rule.length (), stack.getS ().length () - (pos + 1));
	};
	
	auto skip = [&] () {
		int brace = 0;
		while (itt!=end and (brace or (itt->type != lexer::TOKEN::symbol or itt->iden [0] != '}'))) {
			if (itt->type == lexer::TOKEN::symbol and itt->iden [0] == '{') ++brace;
			++itt;
			if (itt->type == lexer::TOKEN::symbol and itt->iden [0] == '}') --brace;
		}
		return itt;
	};
	
	auto shift = [&] () {
		if (next == '{') {
			auto start = itt;
			skip ();
			while (start->type != lexer::TOKEN::symbol and start->iden[0] != '{') ++start;
			stack.push (getCode ("BLOCK"), parseBody (++start, itt));
			if (itt == end or itt+1 == end) return next =';';
			return next = getCode (*(itt+1));
		} else if (next and next != ';') {
			stack.push (next, *(itt+1));
		}
		++itt;
		if (itt == end) return next = ';', '\0';
		if (itt+1 == end) return next = ';';
		return next = getCode (*(itt+1));
	};
	
	if (next == '{') {
		shift ();
	} else {
		if (next) {
			--itt;
		} else {
			if (itt != end and itt+1 != end) {
				next=getCode (*(itt+1));
			} else {
				next=';';
			}
		}
	}
	
	auto getLeaf = [] (const string & str, size_t pos, RuleBranch * branch) -> RuleLeaf * {
		while (pos) {
			--pos;
			RuleBranch * child = (*branch) [str [pos]];
			if (!child) break;
			branch = child;
		}
		return branch->leaf;
	};
	
	PTNode node;
	auto attemptReduce = [&] () {
		RuleLeaf * leaf;
		if (next and (leaf = getLeaf (stack.getS (), stack.getS ().length (), root))) {
			switch (leaf->next [int (next)]) {
				case 'S':
					return 0;
				case 'R':
					reduce (*(leaf->s), leaf->r);
					return 1;
				case 'X':
					node = new ParseTreeError ((itt+1)->line, 0, "Unexpected token: '" + (itt+1)->iden + "'");
					std::cout << "No rule for: '" << (leaf->s ? *(leaf->s) : "") << " " << next << "'\nStack: '" << stack.getS () << "'" << std::endl;
					return -1;
				default:
					if (next != ';' and stack.getS () != "E") {
						node = new ParseTreeError ((itt+1)->line, 0, "Unexpected token: '" + (itt+1)->iden + "'");
						std::cout << "Unexpected token: '" << stack.getS () << " " << next << "' with rule: '" << (leaf->s ? *(leaf->s) : "") << "'" << std::endl;
						return -2;
					}
					return 0;
			}
		} else {
			return 0;
		}
	};
	
	while (itt != end) {
		switch (attemptReduce ()) {
			case -2:
			case -1:
				return node;
			case 0:
				shift ();
		}
	}
	while (attemptReduce ());
	
	if (stack.size () > 1) {
		PTNode n = stack.top ();
		node = new ParseTreeError (n->getLine (), n->getPos (), "Unexpected end of expression");
	} else {
		node = stack.pop ();
	}
	return node;
	
}


void Parser::addRule (const string & rule, const string & ptn, int16_t prec, RuleType ruleType, void * ptr) {
	
	auto tks = lexer::lex (ptn);
	char rc = registerRule (lexer::lexToken(rule));
	string pattern;
	
	for (auto & tkn : tks) {
		switch (tkn.type) {
			case lexer::TOKEN::whitespace:
			case lexer::TOKEN::indent:
				break;
			case lexer::TOKEN::semicolon:
				break;
			default:
				pattern += registerRule (tkn);
				break;
		}
	}
	
	rules [rc].expantions.push_back ({pattern, ptr, prec, ruleType});
	
}

void Parser::addRules (const string & rule, const std::vector <string> & ptn, int16_t prec, int ruleType, void * ptr) {
	for (const string & s : ptn) {
		addRule (rule, s, prec, RuleType (ruleType), ptr);
	}
}

void Parser::buildRules () {
	
	if (root) return;
	root = new RuleBranch ();
	
	std::map <char, std::set <char>> map;
	std::map <string, std::set <char>> shift;
	std::set <string> allRules;
	
	for (auto rule : rules) {
		for (auto expan : rule.second.expantions) {
			allRules.insert (expan.s);
			map [rule.first].insert (expan.s [0]);
			for (size_t i=1; i<expan.s.length (); ++i) {
				shift [expan.s.substr (0, i)].insert (expan.s [i]);
			}
		}
	}
	
	bool dirty=true;
	while (dirty) {
		dirty = false;
		for (auto & m : map) {
			std::set <char> set = m.second;
			for (char c : m.second) {
				set.insert (map [c].begin (), map [c].end ());
			}
			dirty = dirty or set != m.second;
			m.second = set;
		}
	}
	
	dirty = true;
	while (dirty) {
		dirty = false;
		for (auto & s: shift) {
			std::set <char> set = s.second;
			for (char c : s.second) {
				set.insert (map [c].begin (), map [c].end ());
			}
			dirty = dirty or set != s.second;
			s.second = set;
		}
	}
	
	auto getLeafCstr = [&] (const char * str, size_t pos, RuleBranch * branch) -> RuleLeaf * {
		while (pos) {
			--pos;
			RuleBranch * child = (*branch) [str [pos]];
			if (!child) {(*branch) [str [pos]] = child = new RuleBranch ();}
			branch = child;
		}
		if (branch->leaf) {
			return branch->leaf;
		} else {
			return branch->leaf = new RuleLeaf ();
		}
	};
	auto getLeaf = [&] (const string & ptn) {
		return getLeafCstr (ptn.c_str (), ptn.length (), root);
	};
	
	
	for (auto & s : shift) {
		RuleLeaf * leaf = getLeaf (s.first);
		for (char c : s.second) {
			leaf->next [(int)c] = 'S';
			for (char m : map [c]) {
				leaf->next [(int)m] = 'S';
			}
		}
		for (size_t i=s.first.length()-1; i>0; --i) {
			string sub = s.first.substr (i);
			auto itt = shift.find (sub);
			if (itt != shift.end ()) {
				for (char c : itt->second) {
					leaf->next [(int)c] = 'S';
					for (char m : map [c]) {
						leaf->next [(int)m] = 'S';
					}
				}
			}
		}
	}
	
	for (auto rule : rules) {
		for (auto expan : rule.second.expantions) {
			RuleLeaf * leaf = getLeaf (expan.s);
			leaf->r = rule.first;
			leaf->ptr = expan.ptr;
			leaf->s = new string (expan.s);
			leaf->next [';'] = 'R';
			
			auto createReduce = [&] (const string & rule, size_t index) {
				char & c = leaf->next [(int)rule [index]];
				if (c != 'S') {
					c = 'R';
				}
				string r = expan.s;
				while (index+1 < rule.length ()) {
					r += rule [index];
					RuleLeaf * l = getLeaf (r);
					if (not l->s) l->s = new string (expan.s);
					for (char s : map [(int)rule [index+1]]) {
						if (l->next [(int)s] != 'S') l->next [(int)s] = 'R';
					}
					if (l->next [(int)rule [index+1]] != 'S') l->next [(int)rule [index+1]] = 'R';
					if (not l->r) l->r = leaf->r;
					++index;
				}
			};
			
			auto createShift = [&] (const string & rule, size_t index) {
				leaf->next [(int)rule [index]] = 'S';
				for (char s : map [(int)rule [index]]) {
					leaf->next [(int)s] = 'S';
				}
				string r = expan.s;
				while (index+1 < rule.length ()) {
					r += rule [index];
					RuleLeaf * l = getLeaf (r);
					for (char s : map [(int)rule [index+1]]) {
						l->next [(int)s] = 'S';
					}
					l->next [(int)rule [index+1]] = 'S';
					++index;
				}
			};
			
			for (auto comp : rule.second.expantions) {
				if (comp.s.length () >= 2 && comp.s [0] == rule.first) {
					if (comp.prec == expan.prec) {
						if (expan.type & LTR) {
							createReduce (comp.s, 1);
						} else {
							createShift (comp.s, 1);
						}
					} else if (comp.prec > expan.prec) {
						createReduce (comp.s, 1);
					} else {
						createShift (comp.s, 1);
					}
				}
			}
			
			for (size_t c=' '; c<128; ++c) {
				if (leaf->next [(int)c] != 'S') {
					leaf->next [(int)c] = 'R';
				}
			}
		}
	}
	
}

void Parser::pushJob (PTNode * result, numbat::lexer::tkstring::const_iterator itt, numbat::lexer::tkstring::const_iterator end) const {
	
	*result = parseExpr (itt, end);
	
}


Parser::Parser ()  {
	
	memset (tknIdMap, 0, sizeof (tknIdMap));
	registerRule (lexer::lexToken (";"));
	
}


};