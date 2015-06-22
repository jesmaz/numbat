#ifndef FUNCTION_HPP
#define FUNCTION_HPP


#include "base.hpp"

#include <vector>


struct Function : ParseTreeNode {
	
	public:
		
		
		virtual Function * asFunction () {return this;}
		
		Function (uint32_t line, uint32_t pos) : ParseTreeNode (ParseTreeNode::NodeType::FUNCTION, line, pos), body (nullptr) {}
		Function (const std::vector <PTNode> & args);
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		std::vector <PTNode> tplate, tags, params, type;
		string iden;
		PTNode body;
		
};


#endif /*FUNCTION_HPP*/