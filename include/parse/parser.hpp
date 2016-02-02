#ifndef PARSER_HPP
#define PARSER_HPP


#include "../lexer.hpp"
#include "stack.hpp"
#include "tree.hpp"
#include "tree/error.hpp"

#include <bitset>
#include <cstring>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <stack>
#include <vector>


namespace numbat {


struct Parser {
	
	public:
		
		enum RuleType {NONE=0, LTR=1, RTL=2, ACCUM=4};
		
		struct Rule {
			struct Args	 {
				string s;
				std::function <PTNode (const std::vector <PTNode> &)> * ptr;
				int prec;
				RuleType type;
			};
			std::vector <Args> expantions;
		};
		
		union RuleLeaf {
			RuleLeaf () {memset ((void*)(this), 0, sizeof(RuleLeaf));}
			struct {
				std::function <PTNode (const std::vector <PTNode> &)> * ptr;
				string * s;
				char r;
			};
			char next [256];
		};
		
		struct RuleBranch {
			RuleBranch () {memset ((void*)(branch), 0, sizeof(branch));}
			char rule (char c) const {return leaf ? leaf->next [int (c)] : (((c < ' ' or c >= 128) ? branch [127] : branch [int (c - ' ')]) ? 's' : 'x');}
			RuleBranch * & operator [] (char c) {return (c < ' ' or c >= 128) ? branch [127] : branch [int (c - ' ')];}
			RuleLeaf * leaf = nullptr;
			RuleBranch * branch [128 - ' '];
		};
		
		struct ProductionRule {
			struct Expected {
				const Rule::Args * rule;
				char reduce; bool accept;
			};
			string seen;
			const Rule::Args * rule=nullptr;
			std::map <string, Expected> expected;
			bool operator < (const ProductionRule & rhs) const {return rule != rhs.rule ? rule < rhs.rule : seen < rhs.seen;}
		};
		
		struct State {
			enum class Action : uint16_t {ACCEPT='A', ERROR='.', REDUCE='R', SHIFT='S', TRYSHIFT='T'};
			struct R {
				uint32_t index=0;
				Action action=Action::ERROR;
				char c;
			} rules [129 - ' '];
			string ptn;
			std::function <PTNode (const std::vector <PTNode> &)> * ptr=nullptr;
			char reduce=0;
			bool terminal=false;
			R & operator [] (char c) {return (c < ' ' or c >= 128) ? rules [0] : rules [int (c - ' ' + 1)];}
			const R & operator [] (char c) const {return (c < ' ' or c >= 128) ? rules [0] : rules [int (c - ' ' + 1)];}
		};
		
		PTNode parse (const string & prog) {return parse (lexer::lexFile (prog));}
		PTNode parse (const numbat::lexer::tkstring & prog);
		PTNode parse (numbat::lexer::tkstring::const_iterator itt, numbat::lexer::tkstring::const_iterator end) const;
		
		void addRule (const string & rule, const string & ptn, int16_t prec=0, RuleType ruleType=NONE, std::function <PTNode (const std::vector <PTNode> &)> ptr=nullptr);
		void addRules (const string & rule, const std::vector <string> & ptn, int16_t prec=0, int ruleType=NONE, std::function <PTNode (const std::vector <PTNode> &)> ptr=nullptr);
		void buildRules ();
		
		std::map <char, std::map <string, ProductionRule>> seperateKernals (const std::map <string, ProductionRule> & kernals) const;
		std::map <string, ProductionRule> generateKernals () const;
		
		Parser ();
		
	protected:
	private:
		
		char getCode (const string & rule) const;
		char getCode (lexer::TOKEN token) const;
		char getCode (lexer::token tkn) const;
		char registerCode (const lexer::token & tkn, char c=0);
		char registerCode (lexer::TOKEN type, char c=0);
		char registerCode (const string & str, char c=0);
		char registerRule (const lexer::token & tkn);
		
		size_t stateIndex (char reduction, const ProductionRule & rule, const std::map <char, std::map <string, ProductionRule>> & kernals, std::vector <State *> & statePool, std::map <char, std::map <string, uint32_t>> & indexes);
		
		void pushJob (PTNode * result, numbat::lexer::tkstring::const_iterator itt, numbat::lexer::tkstring::const_iterator end) const;
		
		RuleBranch * root = nullptr;
		std::map <char, Rule> rules;
		std::map <string, char> idmap;
		std::bitset <128 - ' '> charsUsed;
		char lastChar = ' '+1;
		char tknIdMap [int (lexer::TOKEN::whitespace) + 1];
		std::vector <State> states;
		size_t initialState=0;
		
};


};

#endif /*PARSER_HPP*/