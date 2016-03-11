#ifndef PARSETREE_HPP
#define PARSETREE_HPP


#include "tree/function.hpp"
#include "tree/struct.hpp"

#include <vector>


class ParseTree : public ParseTreeNode {
	
	public:
		
		const std::vector <Function *> & getFunctions () const {return functions;}
		const std::vector <PTNode> & getBody () const {return body;}
		const std::vector <Struct *> & getStructs () const {return structs;}
		
		numbat::parser::ASTnode build (numbat::parser::NumbatScope * scope);
		
		void declare (numbat::parser::NumbatScope * scope);
		void declareInline ();
		void push_back (PTNode node);
		
		ParseTree (uint32_t line, uint32_t pos) : ParseTreeNode (line, pos) {}
		template <typename container>
		ParseTree (container nodes) : ParseTreeNode (nodes.front ()->getLine (), nodes.front ()->getPos ()) {
			body.reserve (nodes.size ());
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
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		std::vector <PTNode> body, import;
		std::vector <Function *> functions;
		std::vector <Struct *> structs;
		
		numbat::parser::NumbatScope * treeScope=nullptr;
		
};


#endif /*PARSERTREE_HPP*/