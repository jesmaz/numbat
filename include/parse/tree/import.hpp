#pragma once


#include <file.hpp>
#include <parse/tree/base.hpp>


namespace parser {


class ParseTreeImport : public ParseTreeNode {
	
	public:
		
		virtual const string & getIden () const {return iden->getIden ();}
		
		const nir::Instruction * build (nir::Scope * scope);
		
		void declare (nir::Scope * scope);
		
		ParseTreeImport (numbat::lexer::position pos) : ParseTreeNode (pos), path (nullptr), iden (nullptr) {}
		ParseTreeImport (PTNode path, PTNode iden=nullptr) : ParseTreeNode (ParseTreeNode::NodeType::IMPORT, path->getPos ()), path (path), iden (iden) {}
		virtual ~ParseTreeImport () {delete path; delete iden;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		PTNode path, iden;
		numbat::File * sourceFile=nullptr;
		
};

class ParseTreeImportPath : public ParseTreeNode {
	
	public:
		
		virtual bool isAggregate () {return true;}
		virtual const BasicArray <ParseTreeNode *> releaseArgs () {auto a = path; path = BasicArray <PTNode> (); return a;}
		virtual const BasicArray <ParseTreeNode *> & getArgs () const {return path;}
		
		ParseTreeImportPath (numbat::lexer::position pos) : ParseTreeNode (pos) {}
		ParseTreeImportPath (const BasicArray <PTNode> & path) : ParseTreeNode (path [assert (not path.empty ()), 0]->getPos ()), path (path) {}
		virtual ~ParseTreeImportPath () {for (auto p: path) delete p;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		BasicArray <PTNode> path;
		
};


}
