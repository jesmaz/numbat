#ifndef FUNCTION_HPP
#define FUNCTION_HPP


#include "base.hpp"

#include <vector>


struct Function : ParseTreeNode {
	
	public:
		
		const std::vector <PTNode> & getParams () const {return params;}
		const std::vector <PTNode> & getType () const {return type;}
		
		virtual Function * asFunction () {return this;}
		virtual const string & getIden () const {return iden;}
		
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