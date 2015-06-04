#ifndef PARSER_HPP
#define PARSER_HPP


#include "../lexer.hpp"
#include "stack.hpp"
#include "tree.hpp"
#include "tree/error.hpp"

#include <bitset>
#include <cstring>
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
			struct Args {
				string s;
				void * ptr;
				int prec;
				RuleType type;
			};
			std::vector <Args> expantions;
		};
		
		union RuleLeaf {
			RuleLeaf () {memset ((void*)(this), 0, sizeof(RuleLeaf));}
			struct {
				char r;
				void * ptr;
				string * s;
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
		
		PTNode parse (const string & prog) {return parse (lexer::lexFile (prog));}
		PTNode parse (const numbat::lexer::tkstring & prog);
		PTNode parseBody (numbat::lexer::tkstring::const_iterator itt, numbat::lexer::tkstring::const_iterator end) const;
		PTNode parseExpr (numbat::lexer::tkstring::const_iterator itt, numbat::lexer::tkstring::const_iterator end) const;
		
		void addRule (const string & rule, const string & ptn, int16_t prec=0, RuleType ruleType=NONE, void * ptr=nullptr);
		void addRules (const string & rule, const std::vector <string> & ptn, int16_t prec=0, int ruleType=NONE, void * ptr=nullptr);
		void buildRules ();
		
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
		
		void pushJob (PTNode * result, numbat::lexer::tkstring::const_iterator itt, numbat::lexer::tkstring::const_iterator end) const;
		
		RuleBranch * root = nullptr;
		std::map <char, Rule> rules;
		std::map <string, char> idmap;
		std::bitset <128 - ' '> charsUsed;
		char lastChar = ' '+1;
		char tknIdMap [int (lexer::TOKEN::whitespace) + 1];
		
};


};

#endif /*PARSER_HPP*/