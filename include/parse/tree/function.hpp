#ifndef FUNCTION_HPP
#define FUNCTION_HPP


#include "base.hpp"

#include <vector>


struct Function : ParseTreeNode {
	
	public:
		
		const std::vector <PTNode> & getParams () const {return params;}
		const std::vector <PTNode> & getType () const {return type;}
		
		const nir::Instruction * build (nir::Scope * scope, BuildMode mode);
		
		void declare (nir::Scope * scope);
		void setBody (PTNode b) {body = b;}
		
		virtual Function * asFunction () {return this;}
		virtual const string & getIden () const {return iden;}
		
		Function (uint32_t line, uint32_t pos) : ParseTreeNode (ParseTreeNode::NodeType::FUNCTION, line, pos), body (nullptr) {}
		Function (PTNode iden, PTNode params, PTNode type, PTNode body);
		Function (uint32_t line, uint32_t pos, const string & iden, const std::vector <PTNode> & params, const std::vector <PTNode> & type, PTNode body, nir::LINKAGE linkage);
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		std::vector <PTNode> tplate, tags, params, type;
		string iden;
		PTNode body;
		nir::LINKAGE linkage;
		
		nir::Scope * fScope=nullptr;
		
};


#endif /*FUNCTION_HPP*/