#ifndef PARSETREE_HPP
#define PARSETREE_HPP


#include "tree/function.hpp"
#include "tree/struct.hpp"

 


class ParseTree : public ParseTreeNode {
	
	public:
		
		const BasicArray <Function *> & getFunctions () const {return functions;}
		const BasicArray <PTNode> & getBody () const {return body;}
		const BasicArray <Struct *> & getStructs () const {return structs;}
		
		const nir::Instruction * build (nir::Scope * scope);
		
		void declare (nir::Scope * scope);
		void declareInline ();
		void push_back (PTNode node);
		
		ParseTree (uint32_t line, uint32_t pos) : ParseTreeNode (line, pos) {}
		template <typename container>
		ParseTree (container nodes) : ParseTreeNode (nodes.front ()->getLine (), nodes.front ()->getPos ()) {
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
		
		DynArray <PTNode> body, import;
		DynArray <Function *> functions;
		DynArray <Struct *> structs;
		
		nir::Scope * nirTreeScope=nullptr;
		
};


#endif /*PARSERTREE_HPP*/
