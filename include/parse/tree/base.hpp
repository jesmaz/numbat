#ifndef PARSETREENODE_HPP
#define PARSETREENODE_HPP


#include <string>


using std::string;

class ParseTreeNode {
	
	public:
		
		string toString () {if (this) return strDump (); else return "nullptr";}
		
		uint32_t getLine () {return line;}
		uint32_t getPos () {return pos;}
		
		ParseTreeNode (uint32_t line, uint32_t pos) : line (line), pos (pos) {}
		virtual ~ParseTreeNode () {}
		
	protected:
	private:
		
		ParseTreeNode (ParseTreeNode & other) {}
		virtual ParseTreeNode & operator = (const ParseTreeNode & rhs) {return *this;}
		
		virtual string strDump ()=0;
		
		uint32_t line, pos;
		
};

typedef ParseTreeNode * PTNode;


#endif /*PARSETREENODE_HPP*/