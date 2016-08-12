#ifndef PARSETREENODE_HPP
#define PARSETREENODE_HPP


#include <cassert>
#include <iostream>
#include <nir/forward.hpp>
#include <string>
#include <typeinfo>
#include <utility/text.hpp>
#include <vector>


using std::string;

struct Function;
struct Struct;

class ParseTreeNode {
	
	public:
		
		enum class NodeType {EXPRESSION, FUNCTION, IMPORT, KEYWORD, LIST, STRUCT, SYMBOL};
		
		NodeType getType () {return type;}
		
		string toString (text::PrintMode mode=text::PLAIN) {if (this) return strDump (mode); else return "nullptr";}
		
		uint32_t getLine () {return line;}
		uint32_t getPos () {return pos;}
		
		virtual bool isAggregate () {return false;}
		virtual bool isList () {return false;}
		virtual const std::vector <ParseTreeNode *> & getArgs () const {return defaultArgs;}
		virtual const std::vector <ParseTreeNode *> releaseArgs () {return defaultArgs;}
		virtual const std::vector <ParseTreeNode *> & getTags () const {return defaultArgs;}
		virtual const std::vector <ParseTreeNode *> & getTemplate () const {return defaultArgs;}
		virtual const string & getIden () const {return defaultStr;}
		virtual Function * asFunction () {return nullptr;}
		virtual Struct * asStruct () {return nullptr;}
		
		virtual const nir::Instruction * build (nir::Scope * scope) {std::cerr << "virtual 'build' not implemented for: " << typeid (*this).name () << std::endl; assert (0); return nullptr;}
		virtual const nir::Instruction * buildAllocate (nir::Scope * scope, const string & iden) {std::cerr << typeid (*this).name () << " can't create an allocate instruction" << std::endl; assert (0); return nullptr;}
		virtual const nir::Instruction * buildParameter (nir::Scope * scope) {std::cerr << typeid (*this).name () << " is not a valid parameter" << std::endl; assert (0); return nullptr;}
		virtual const nir::Type * resolveType (nir::Scope * scope) {std::cerr << typeid (*this).name () << " can't resolve a type" << std::endl; assert (0); return nullptr;}
		
		virtual void declare (nir::Scope * scope) {std::cerr << "virtual 'declare' not implemented for: " << typeid (*this).name () << std::endl; assert (0);}
		virtual void push_back (ParseTreeNode * e) {std::cerr << "virtual 'push_back' not implemented for: " << typeid (*this).name () << std::endl; assert (0);}
		
		ParseTreeNode (uint32_t line, uint32_t pos) : type (NodeType::EXPRESSION), line (line), pos (pos) {}
		ParseTreeNode (NodeType nodeType, uint32_t line, uint32_t pos) : type (nodeType), line (line), pos (pos) {}
		virtual ~ParseTreeNode () {}
		
	protected:
	private:
		
		ParseTreeNode (ParseTreeNode & other) {}
		virtual ParseTreeNode & operator = (const ParseTreeNode & rhs) {return *this;}
		
		virtual string strDump (text::PrintMode mode)=0;
		
		NodeType type=NodeType::EXPRESSION;
		uint32_t line, pos;
		const static std::vector <ParseTreeNode *> defaultArgs;
		const static string defaultStr;
		
};

typedef ParseTreeNode * PTNode;


#endif /*PARSETREENODE_HPP*/