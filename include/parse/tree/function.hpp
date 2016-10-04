#pragma once


#include <parse/tree/base.hpp>


namespace parser {


struct Function : ParseTreeNode {
	
	public:
		
		const BasicArray <PTNode> & getParams () const {return params;}
		const BasicArray <PTNode> & getType () const {return type;}
		
		const nir::Instruction * build (nir::Scope * scope);
		
		void declare (nir::Scope * scope);
		void setBody (PTNode b) {body = b;}
		
		virtual Function * asFunction () {return this;}
		virtual const string & getIden () const {return iden;}
		
		Function (numbat::lexer::position pos) : ParseTreeNode (ParseTreeNode::NodeType::FUNCTION, pos), body (nullptr) {}
		Function (PTNode iden, PTNode params, PTNode type, PTNode body);
		Function (numbat::lexer::position pos, const string & iden, const BasicArray <PTNode> & params, const BasicArray <PTNode> & type, PTNode body, nir::LINKAGE linkage);
		
		virtual ~Function () {delAll (tplate); delAll (tags); delAll (params); delAll (type); delete body;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		BasicArray <PTNode> tplate, tags, params, type;
		string iden;
		PTNode body;
		nir::LINKAGE linkage;
		
		nir::Scope * fScope=nullptr;
		
};


}
