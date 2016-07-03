#ifndef NIR_SCOPE
#define NIR_SCOPE


#include <file.hpp>
#include <map>
#include <nir/instruction.hpp>
#include <set>
#include <vector>


namespace nir {

using std::string;

struct Scope {
	
	public:
		
		const Function * getOwner () const {if (owner) return owner; else if (parent) return parent->getOwner (); else return nullptr;}
		Module * getModule () const {return module;}
		const numbat::File * getSourceFile () const {return sourceFile;}
		
		Scope * declareFunction (const std::vector <const Parameter *> params, const std::vector <const Parameter *> ret, const string iden="");
		
		symbol createBlock (const string & iden="");
		size_t changeActiveBlock (symbol block);
		
		const Type * resolveType (const string & iden) const;
		
		Argument createGet (Argument src);
		Argument staticCast (Argument src, const Type * const target, const string & iden="");
		
		const Instruction * allocateVariable (const Type * const type, const string & iden="");
		template <typename T>
		const Instruction * createBinary (Argument lhs, Argument rhs, const string & iden);
		const Instruction * createAdd (const std::vector <Argument> & args);
		const Instruction * createAssign (const std::vector <Argument> & args);
		const Instruction * createCall (const Function * func, const std::vector <Argument> & args);
		template <typename T>
		const Instruction * createCmp (Argument lhs, Argument rhs, const string & iden);
		const Instruction * createCmpLT (const std::vector <Argument> & args);
		const Instruction * createConstant (const Type * type, const string & val, const string & iden="");
		const Instruction * createDiv (const std::vector <Argument> & args);
		const Instruction * createImportHandle (const Scope * scope, const string & iden="");
		const Instruction * createNeg (const std::vector <Argument> & args);
		const Instruction * createJump (symbol block);
		const Instruction * createJump (Argument cond, symbol block);
		const Instruction * createMul (const std::vector <Argument> & args);
		const Instruction * createParameter (const Type * const type, Argument init={nullptr, nullptr}, const string & iden="");
		const Instruction * createPut (Argument src, Argument dest);
		const Instruction * createStructValue (const Type * const type, std::vector <Argument> vals, const string & iden="");
		const Instruction * createSub (const std::vector <Argument> & args);
		const Instruction * getFunctionPointer ();
		const Instruction * resolve (const string & iden);
		const Instruction * staticCast (const Instruction * src, const Type * const target, const string & iden="");
		
		void declareFunction ();
		
		friend Module;
		
	protected:
	private:
		
		Scope (Module * mod, Block * insertionPoint, Function * owner) : module (mod), insertionPoint (insertionPoint), owner (owner) {}
		
		std::map <string, std::vector <Function *> *> functions;
		std::map <string, Type *> types;
		std::map <string, const Instruction *> variables;
		std::map <symbol, Block *> blocks;
		std::set <Scope *> scopes;
		Scope * parent=nullptr;
		Module * module;
		Block * insertionPoint;
		Function * owner;
		numbat::File * sourceFile;
		
	
};

};


#endif/*NIR_SCOPE*/