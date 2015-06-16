#ifndef PARSETREENODE_HPP
#define PARSETREENODE_HPP


#include <string>


using std::string;

struct Function;
struct Struct;

class ParseTreeNode {
	
	public:
		
		enum class NodeType {EXPRESSION, FUNCTION, STRUCT};
		
		NodeType getType () {return type;}
		
		string toString () {if (this) return strDump (); else return "nullptr";}
		
		uint32_t getLine () {return line;}
		uint32_t getPos () {return pos;}
		
		virtual bool isAggregate () {return false;}
		virtual Function * asFunction () {return nullptr;}
		virtual Struct * asStruct () {return nullptr;}
		
		ParseTreeNode (uint32_t line, uint32_t pos) : type (NodeType::EXPRESSION), line (line), pos (pos) {}
		ParseTreeNode (NodeType nodeType, uint32_t line, uint32_t pos) : type (nodeType), line (line), pos (pos) {}
		virtual ~ParseTreeNode () {}
		
	protected:
	private:
		
		ParseTreeNode (ParseTreeNode & other) {}
		virtual ParseTreeNode & operator = (const ParseTreeNode & rhs) {return *this;}
		
		virtual string strDump ()=0;
		
		NodeType type;
		uint32_t line, pos;
		
};

typedef ParseTreeNode * PTNode;


#endif /*PARSETREENODE_HPP*/