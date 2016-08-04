#ifndef IMPORT_HPP
#define IMPORT_HPP


#include "base.hpp"
#include <file.hpp>

#include <vector>


class ParseTreeImport : public ParseTreeNode {
	
	public:
		
		virtual const string & getIden () const {return iden->getIden ();}
		
		const nir::Instruction * build (nir::Scope * scope, BuildMode mode);
		
		void declare (nir::Scope * scope);
		
		ParseTreeImport (uint32_t line, uint32_t pos) : ParseTreeNode (line, pos), path (nullptr), iden (nullptr) {}
		ParseTreeImport (PTNode path, PTNode iden=nullptr) : ParseTreeNode (ParseTreeNode::NodeType::IMPORT, path->getLine (), path->getPos ()), path (path), iden (iden) {}
		virtual ~ParseTreeImport () {delete path; delete iden;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		PTNode path, iden;
		numbat::File * sourceFile;
		
};

class ParseTreeImportPath : public ParseTreeNode {
	
	public:
		
		virtual bool isAggregate () {return true;}
		virtual const std::vector <ParseTreeNode *> releaseArgs () {auto a = path; path = std::vector <PTNode> (); return a;}
		virtual const std::vector <ParseTreeNode *> & getArgs () const {return path;}
		
		ParseTreeImportPath (uint32_t line, uint32_t pos) : ParseTreeNode (line, pos) {}
		ParseTreeImportPath (const std::vector <PTNode> & path) : ParseTreeNode (path[assert (not path.empty ()), 0]->getLine (), path[0]->getPos ()), path (path) {}
		virtual ~ParseTreeImportPath () {for (auto p: path) delete p;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		std::vector <PTNode> path;
		
};


#endif /*IMPORT_HPP*/