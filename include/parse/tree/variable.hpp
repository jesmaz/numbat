#ifndef PARSETREEVARIABLE_HPP
#define PARSETREEVARIABLE_HPP


#include "base.hpp"


class ParseTreeVariable : public ParseTreeNode {
	
	public:
		
		virtual const nir::Instruction * build (nir::Scope * scope);
		virtual const nir::Instruction * buildParameter (nir::Scope * scope);
		
		PTNode releaseIden () {auto e = iden; iden = 0; return e;}
		PTNode releaseInst () {auto e = inst; inst = 0; return e;}
		PTNode releaseVType () {auto e = vType; vType = 0; return e;}
		
		ParseTreeVariable (uint32_t line, uint32_t pos) : ParseTreeNode (line, pos) {}
		ParseTreeVariable (PTNode vType, PTNode iden) : ParseTreeNode (iden->getLine (), iden->getPos ()), vType (vType), iden (iden), inst (nullptr) {}
		ParseTreeVariable (PTNode vType, PTNode iden, PTNode inst) : ParseTreeNode (iden->getLine (), iden->getPos ()), vType (vType), iden (iden), inst (inst) {}
		
		virtual ~ParseTreeVariable () {delete vType; delete iden; delete inst;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode);
		
		PTNode vType=nullptr, iden=nullptr, inst=nullptr;
		
};


#endif /*PARSETREEVARIABLE_HPP*/
