#pragma once


#include <forward.hpp>
#include <map>
#include <nir/instruction.hpp>
#include <nir/type/struct.hpp>
#include <set>
#include <token.hpp>
 


namespace nir {

using std::string;

struct Scope {
	
	public:
		
		Block * getCurrentBlock () const {return insertionPoint;}
		const Function * getOwner () const {if (owner) return owner; else if (parent) return parent->getOwner (); else return nullptr;}
		Module * getModule () const {return module;}
		const numbat::File * getSourceFile () const {return sourceFile;}
		
		Scope * declareFunction (const BasicArray <const Parameter *> params, const BasicArray <const Parameter *> ret, const string iden, LINKAGE linkage);
		
		symbol createBlock (const string & iden="");
		size_t changeActiveBlock (Block * block) {return insertionPoint = block, 0;}
		size_t changeActiveBlock (symbol block);
		
		const Type * resolveType (Argument parent, const string & iden, numbat::lexer::position pos) const;
		const Type * resolveType (const string & iden, numbat::lexer::position pos) const;
		
		Argument loadReference (Argument arg);
		Argument loadValue (Argument arg);
		Argument staticCast (Argument src, const Type * const target, const string & iden="");
		
		const Instruction * allocateArray (const Type * const type, Argument size, const string & iden="");
		const Instruction * allocateBytes (const Type * const type, Argument size, const string & iden="");
		const Instruction * allocateVariable (const Type * const type, const string & iden="");
		template <typename T>
		const Instruction * createBinary (Argument lhs, Argument rhs, const string & iden);
		const Instruction * createAdd (Argument lhs, Argument rhs);
		const Instruction * createAssign (Argument lhs, Argument rhs);
		const Instruction * createAutoReturn (const Instruction * instr);
		const Instruction * createBitAnd (Argument lhs, Argument rhs);
		const Instruction * createBitNot (Argument arg);
		const Instruction * createBitOr (Argument lhs, Argument rhs);
		const Instruction * createBitXor (Argument lhs, Argument rhs);
		const Instruction * createCall (const Function * func, const BasicArray <Argument> & args);
		template <typename T>
		const Instruction * createCmp (Argument lhs, Argument rhs, const string & iden);
		const Instruction * createCmpEQ (Argument lhs, Argument rhs);
		const Instruction * createCmpGT (Argument lhs, Argument rhs);
		const Instruction * createCmpGTE (Argument lhs, Argument rhs);
		const Instruction * createCmpLT (Argument lhs, Argument rhs);
		const Instruction * createCmpLTE (Argument lhs, Argument rhs);
		const Instruction * createCmpNE (Argument lhs, Argument rhs);
		const Instruction * createConstant (const Type * type, const string & val, numbat::lexer::position pos, const string & iden="");
		const Instruction * createDiv (Argument lhs, Argument rhs);
		const Instruction * createGet (Argument src);
		const Instruction * createImportHandle (const Scope * scope, const string & iden="");
		const Instruction * createJump (symbol block);
		const Instruction * createJump (Argument cond, symbol block);
		const Instruction * createLNot (Argument arg);
		const Instruction * createMul (Argument lhs, Argument rhs);
		const Instruction * createNeg (Argument arg);
		const Instruction * createParameter (const Type * const type, Argument init = Argument (), const string & iden="");
		const Instruction * createPointerAdd (const Type * const type, Argument ptr, Argument offset, const string & iden="");
		const Instruction * createPut (Argument src, Argument dest);
		const Instruction * createReinterpret (Argument ptr, const Type * type, const string & iden="");
		const Instruction * createRem (Argument lhs, Argument rhs);
		const Instruction * createStructValue (const Type * const type, BasicArray <Argument> vals, const string & iden="");
		const Instruction * createSub (Argument lhs, Argument rhs);
		const Instruction * getFunctionPointer ();
		const Instruction * resolve (Argument parent, const string & iden, numbat::lexer::position pos);
		const Instruction * resolve (const string & iden, numbat::lexer::position pos);
		const Instruction * resolve (const string & iden, Block * insertionPoint, numbat::lexer::position pos) const;
		const Instruction * staticCast (const Type * src, const Type * const target, const string & iden="");
		
		Struct * registerStruct (const std::string & iden, numbat::lexer::position pos);
		
		void declareFunction ();
		
		friend Module;
		
	protected:
	private:
		
		Scope (Module * mod, Block * insertionPoint, Function * owner, const numbat::File * sourceFile) : module (mod), insertionPoint (insertionPoint), owner (owner), sourceFile (sourceFile) {}
		
		std::map <string, DynArray <Function *> *> functions;
		std::map <string, Type *> types;
		std::map <string, const Instruction *> variables;
		std::map <symbol, Block *> blocks;
		std::set <Scope *> scopes;
		Scope * parent=nullptr;
		Module * module=nullptr;
		Block * insertionPoint=nullptr;
		Function * owner=nullptr;
		const numbat::File * sourceFile=nullptr;
		
	
};

}
