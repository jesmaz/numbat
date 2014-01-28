#ifndef ABSTRACT_SYNTAX_TREE_HPP
#define ABSTRACT_SYNTAX_TREE_HPP

#include "ast/astbody.hpp"
#include "ast/astcall.hpp"
#include "ast/astcallerror.hpp"
#include "ast/astcallindex.hpp"
#include "ast/astconstantint.hpp"
#include "ast/asterror.hpp"
#include "ast/astfunctionpointer.hpp"
#include "ast/astnumbatinstr.hpp"
#include "ast/astparamater.hpp"
#include "ast/astrawdata.hpp"
#include "ast/astreturn.hpp"
#include "ast/aststructindex.hpp"
#include "ast/asttuple.hpp"
#include "ast/asttuplecall.hpp"
#include "ast/asttype.hpp"
#include "ast/astvariable.hpp"
#include "ast/functiondecleration.hpp"
#include "ast/numbatrawtype.hpp"
#include "ast/numbattype.hpp"
#include "ast/numbatvariable.hpp"
#include "ast/operatordecleration.hpp"
#include "lexer.hpp"
#include "module.hpp"
#include "token.hpp"
#include "visitor.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <unordered_set>
#include <vector>

using std::shared_ptr;
using std::string;
using std::unique_ptr;

namespace numbat {
namespace parser {


const size_t pointerSize = 8;

typedef lexer::tkstring::const_iterator tkitt;


struct AbstractSyntaxTree {
	
	public:
		const bool buildSuccessfull () const {return !buildFail;}
		const std::map <string, shared_ptr <NumbatType>> & getTypes () const {return types;}
		const std::multimap <string, shared_ptr <FunctionDecleration>> & getFunctions () const {return functions;}
		const std::map <string, shared_ptr <OperatorDecleration>> & getOperators () const {return operators;}
		string toString (const string & indent = "") const {
			string str="";
			for (auto & itt : types) {
				str += itt.first + " : " + itt.second->toString (indent) + "\n";
			}
			str += '\n';
			for (auto & itt : functions) {
				str += indent + "'" + itt.first + "' " + itt.second->toString (indent);
			}
			return str;
		}
		
		AbstractSyntaxTree () {}
		AbstractSyntaxTree (tkitt beg, tkitt end);
		
	private:
		
		ASTnode createBinaryCall (const string & func, const ASTnode & lhs, const ASTnode & rhs, tkitt end);
		ASTnode createCallNode (const shared_ptr <ASTcallable> & callee, const std::vector <ASTnode> & args);
		ASTnode createStaticCast (const ASTnode & arg, const ASTnode & type, int maxDepth=1);
		ASTnode createTuple (const ASTnode & lhs, const ASTnode & rhs);
		ASTnode parseAssembly (const string & type, const string & code);
		ASTnode parseBody (tkitt end); // TODO: needs template info parem
		ASTnode parseExpression (tkitt end);
		ASTnode parseExpression (std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end, const std::vector <ASTnode> * args = nullptr);
		ASTnode parseOperator (const OperatorDecleration & opp, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt softEnd, tkitt end, const std::vector <ASTnode> * prevArgs = nullptr);
		ASTnode parseParameter (tkitt end);
		ASTnode parsePrimaryExpression (tkitt end) {return parsePrimaryExpression (end, nullptr);}
		ASTnode parsePrimaryExpression (tkitt end, const std::vector <ASTnode> * args);
		ASTnode parseStatment (tkitt end);
		ASTnode parseType (tkitt end);
		
		bool eatSemicolon (tkitt end) {if (itt->type == lexer::TOKEN::semicolon) return nextToken (end); return false;}
		bool flushLine (tkitt end);
		bool nextToken (tkitt end);
		bool parseFunctionPrototype (std::vector <ASTnode> & args, string & iden, tkitt end);
		
		FunctionDecleration * parseFunctionDecleration (tkitt end);
		
		OperatorDecleration::OperatorMatch matchFromRight (const string & patern, tkitt beg, tkitt end);
		
		shared_ptr <ASTcallable> findFunction (const string & iden, const std::vector <ASTnode> & args, int convert=1);
		
		shared_ptr <NumbatType> generateRawType (const string & key, size_t size, const std::set <string> & metaArgs);
		
		size_t parseIntLiteral (tkitt end);
		
		std::list <OperatorDecleration::OperatorMatch> generateOperatorMatches (tkitt end);
		
		std::set <string> parseMetaTags (tkitt end) {std::set <string> ret; while (itt->type == lexer::TOKEN::atsym and nextToken (end)) {ret.insert (itt->iden); nextToken (end);} return ret;}
		
		std::vector <ASTnode> createCallIndexList (const shared_ptr <ASTcallable> & call);
		std::vector <ASTnode> createStaticCast (const std::vector <ASTnode> & args, const std::vector <ASTnode> & types, int maxDepth=1);
		std::vector <ASTnode> parseArgs (ASTnode (AbstractSyntaxTree::*arg)(tkitt), tkitt end);
		std::vector <ASTnode> parseTemplateArgs (tkitt end);
		
		string parseStructDecleration (tkitt end);
		
		tkitt findIndent (const int level, tkitt end);
		tkitt findLastToken (const string & token, tkitt end) {return findLastToken (token, itt, end);}
		tkitt findLastToken (const string & token, tkitt beg, tkitt end);
		tkitt findToken (const string & token, tkitt end) {return findToken (token, itt, end);}
		tkitt findToken (const string & token, tkitt beg, tkitt end);
		
		void addOperator (const string & pattern, const OperatorDecleration & oppdec);
		void error (const string & message, tkitt end) {printError (message); flushLine (end);}
		void printError (const string & message) {buildFail = true; std::cerr << "error on line " << line << ": " << message << '\n';}
		void parseImport (tkitt end);
		void parseOperatorDecleration (tkitt end);
		void parseTypeDef (tkitt end);
		
		
		
		
		bool buildFail = false;
		
		int indentLevel=0;
		
		tkitt itt;
		
		size_t line = 0;
		
		std::map <string, shared_ptr <NumbatType>> types;
		std::map <string, shared_ptr <NumbatVariable>> variables;
		std::map <string, shared_ptr <OperatorDecleration>> operators;
		
		std::multimap <string, shared_ptr <FunctionDecleration>> functions;
		std::multimap <string, shared_ptr <OperatorDecleration>> operatorsByFirstToken;
		
		std::set <shared_ptr <OperatorDecleration>, std::greater <shared_ptr <OperatorDecleration>>> precidenceOrderedOperators;
		
		std::unordered_set <string> parenOpperators, oppTokens, ternaryStart;
		
};


std::vector <ASTnode> expandNodes (const std::vector <ASTnode> & args);

void splitListAboutTkn (std::list <OperatorDecleration::OperatorMatch> & lhs, std::list <OperatorDecleration::OperatorMatch> & source, tkitt);


};
};

#endif /* ABSTRACT_SYNTAX_TREE_HPP */
