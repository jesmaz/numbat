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
		
	protected:
	private:
		
		virtual string strDump ()=0;
		
		uint32_t line, pos;
		
};

typedef ParseTreeNode * PTNode;


#endif /*PARSETREENODE_HPP*/