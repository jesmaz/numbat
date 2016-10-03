#pragma once


#include <cassert>
#include <iostream>
#include <nir/forward.hpp>
#include <string>
#include <token.hpp>
#include <typeinfo>
#include <utility/array.hpp>
#include <utility/text.hpp>
 


using std::string;

struct Function;
struct Struct;

class ParseTreeNode {
	
	public:
		
		enum class NodeType {EXPRESSION, FUNCTION, IMPORT, KEYWORD, LIST, STRUCT, SYMBOL};
		
		NodeType getType () {return type;}
		
		string toString (text::PrintMode mode=text::PLAIN) {if (this) return strDump (mode); else return "nullptr";}
		
		numbat::lexer::position getPos () {return pos;}
		
		virtual bool isAggregate () {return false;}
		virtual bool isList () {return false;}
		virtual const BasicArray <ParseTreeNode *> & getArgs () const {return defaultArgs;}
		virtual const BasicArray <ParseTreeNode *> releaseArgs () {return defaultArgs;}
		virtual const BasicArray <ParseTreeNode *> & getTags () const {return defaultArgs;}
		virtual const BasicArray <ParseTreeNode *> & getTemplate () const {return defaultArgs;}
		virtual const string & getIden () const {return defaultStr;}
		virtual Function * asFunction () {return nullptr;}
		virtual Struct * asStruct () {return nullptr;}
		
		virtual const nir::Instruction * build (nir::Scope * scope);
		virtual const nir::Instruction * buildAllocate (nir::Scope * scope, const string & iden);
		virtual const nir::Instruction * buildParameter (nir::Scope * scope);
		virtual const nir::Type * resolveType (nir::Scope * scope);
		
		virtual void declare (nir::Scope * scope);
		virtual void push_back (ParseTreeNode * e);
		
		ParseTreeNode (numbat::lexer::position pos) : type (NodeType::EXPRESSION), pos (pos) {}
		ParseTreeNode (NodeType nodeType, numbat::lexer::position pos) : type (nodeType), pos (pos) {}
		virtual ~ParseTreeNode () {}
		
	protected:
	private:
		
		ParseTreeNode (ParseTreeNode & other) {}
		virtual ParseTreeNode & operator = (const ParseTreeNode & rhs) {return *this;}
		
		virtual string strDump (text::PrintMode mode)=0;
		
		NodeType type=NodeType::EXPRESSION;
		numbat::lexer::position pos;
		const static BasicArray <ParseTreeNode *> defaultArgs;
		const static string defaultStr;
		
};

typedef ParseTreeNode * PTNode;
