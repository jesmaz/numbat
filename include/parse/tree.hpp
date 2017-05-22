#pragma once


#include <parse/tree/base.hpp>


namespace parser {


class ParseTree : public ParseTreeNode {
	
	public:
		
		AST::Context * createContext (const AST::Context &);
		AST::NodePtr createAST (AST::Context & ctx);
		
		const BasicArray <Function *> & getFunctions () const {return functions;}
		const BasicArray <PTNode> & getBody () const {return body;}
		const BasicArray <Struct *> & getStructs () const {return structs;}
		
		const nir::Instruction * build (nir::Scope * scope);
		
		void declare (nir::Scope * scope);
		void declareInline ();
		void push_back (PTNode node);
		
		ParseTree (numbat::lexer::position pos) : ParseTreeNode (pos) {}
		template <typename container>
		ParseTree (container nodes) : ParseTreeNode (nodes.front ()->getPos ()) {
			for (PTNode node : nodes) {
				switch (node->getType ()) {
					case ParseTreeNode::NodeType::FUNCTION:
						functions.push_back (node->asFunction ());
						break;
					case ParseTreeNode::NodeType::IMPORT:
						import.push_back (node);
						break;
					case ParseTreeNode::NodeType::STRUCT:
						structs.push_back (node->asStruct ());
						break;
					default:
						break;
				}
				body.push_back (node);
			}
		}
		
		virtual ~ParseTree () {delAll (body);}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		DynArray <PTNode> body, import;
		DynArray <Function *> functions;
		DynArray <Struct *> structs;
		
		nir::Scope * nirTreeScope=nullptr;
		std::unique_ptr <AST::Context> context=nullptr;
		
};


}
