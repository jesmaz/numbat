#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <llvm/Analysis/Passes.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/IR/Attributes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/PassManager.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/Scalar.h>

#include "ast.hpp"
#include "module.hpp"
#include "token.hpp"

#include <iostream>
#include <map>
#include <memory>
#include <stack>
#include <vector>


namespace numbat {
namespace parser {

using namespace llvm;
using visitor::Visitor;


class BodyGenerator : public Visitor <ASTnumbatInstr>, public Visitor <ASTallocate>, public Visitor <ASTbody>, public Visitor <ASTbranch>, public Visitor <ASTcall>, public Visitor <ASTcallindex>, public Visitor <ASTconcat>, public Visitor <ASTconstantFPInt>, public Visitor <ASTconstantInt>, public Visitor <ASTconstantCString>, public Visitor <ASTfunctionPointer>, public Visitor <ASTgep>, public Visitor <ASTmemcpy>, public Visitor <ASTparamater>, public Visitor <ASTrawdata>, public Visitor <ASTreinterpretCast>, public Visitor <ASTreturn>, public Visitor <ASTreturnvoid>, public Visitor <ASTstructIndex>, public Visitor <ASTtuple>, public Visitor <ASTtuplecall>, public Visitor <ASTtype>, public Visitor <ASTvariable>, public Visitor <ASTwhileloop>, public Visitor <NumbatScope> {
	public:
		Value * getValue (const ASTnode & node) {return getValue (node.get ());}
		Value * getValue (ASTbase * node, bool ref=false);
		
		virtual void visit (AbstractSyntaxTree & exp);
		virtual void visit (ASTallocate & exp);
		virtual void visit (ASTbody & exp);
		virtual void visit (ASTbranch & exp);
		virtual void visit (ASTcall & exp);
		virtual void visit (ASTcallindex & exp);
		virtual void visit (ASTconcat & exp);
		virtual void visit (ASTconstantFPInt & exp);
		virtual void visit (ASTconstantInt & exp);
		virtual void visit (ASTconstantCString & exp);
		virtual void visit (ASTfunctionPointer & exp) {}
		virtual void visit (ASTgep & exp);
		virtual void visit (ASTmemcpy & exp);
		virtual void visit (ASTnumbatInstr & exp);
		virtual void visit (ASTparamater & exp);
		virtual void visit (ASTrawdata & exp) {}
		virtual void visit (ASTreinterpretCast & exp);
		virtual void visit (ASTreturn & exp);
		virtual void visit (ASTreturnvoid & exp);
		virtual void visit (ASTstructIndex & exp);
		virtual void visit (ASTtuple & exp);
		virtual void visit (ASTtuplecall & exp);
		virtual void visit (ASTtype & exp) {}
		virtual void visit (ASTvariable & exp);
		virtual void visit (ASTwhileloop & exp);
		virtual void visit (NumbatScope & exp);
		void visit (const shared_ptr <Module> & module);
		void finalize () {diBuilder.finalize ();}
		//std::vector <Value *> operator () (AbstractSyntaxTree & ast) {tree = &ast; ast.getBody ().accept (*this); return body;}
		BodyGenerator (llvm::Module * mod, const DataLayout * dataLayout, FunctionPassManager * fpm=nullptr) : breakBlock (nullptr), continueBlock (nullptr), dataLayout (dataLayout), diBuilder (*mod), activeFunction (nullptr), main (nullptr), memalloc (nullptr), memfree (nullptr), builder (getGlobalContext ()), context (getGlobalContext ()), module (mod), fpm (fpm) {}
	protected:
	private:
		
		struct stackVal {
			enum valType {TYPE, VALUE};
			valType type;
			union {
				Type * typePtr;
				Value * valuePtr;
			};
			Type * asType () {return type == TYPE ? typePtr : nullptr;}
			Value * asValue () {return type == VALUE ? valuePtr : nullptr;}
			stackVal (Type * ptr) : type (TYPE), typePtr (ptr) {}
			stackVal (Value * ptr) : type (VALUE), valuePtr (ptr) {}
		};
		
		DIFile getDIFile (const ParsingContext * context);
		Function * getFunction (const FunctionDecleration * func);
		Type * getType (const ASTnode & node);
		Type * getType (const NumbatType * type);
		Value * allocteArray (Value * length, const NumbatPointerType * type);
		Value * createTemp (std::vector <Value *> vals, bool alloc=true);
		Value * createTemp (Value * val);
		Value * getVariableHandle (const NumbatVariable * var);
		Value * initialise (const NumbatType * var);
		Value * makeCompare (Value * val);
		Value * pointerTypeGEP (Value * ptr, const NumbatPointerType * ptrType, size_t index);
		void createMemCpy (Value * dest, Value * source, Value * length, const shared_ptr <ASTcallable> & conv);
		Value * makeCompare (const ASTnode & exp);
		
		AbstractSyntaxTree * tree;
		BasicBlock * breakBlock, * continueBlock;
		const DataLayout * dataLayout;
		DIBuilder diBuilder;
		Function * activeFunction, * main, * memalloc, * memfree;
		const FunctionDecleration * activeFunctionDecleration;
		IRBuilder<> builder;
		LLVMContext & context;
		llvm::Module * module;
		FunctionPassManager * fpm;
		std::map <const ASTbase *, Value *> tempValues;
		std::map <const ASTbase *, Value *> values;
		std::map <const FunctionDecleration *, Function *> functions;
		std::map <const ParsingContext *, DIFile> diFiles;
		std::map <const NumbatType *, Type *> structures;
		std::map <const NumbatVariable *, Value *> namedValues;
		std::map <string, Value *> strConstants;
		//std::stack <stackVal> stack;
		std::set <Module *> builtModules;
		std::stack <Value *> stack;
		std::vector <Value *> body;
		//CallMeta callmeta;
		struct {
			DICompileUnit cu;
			DIDescriptor scope;
			DIFile file;
			size_t line;
		} debugContext;
};


};
};


#endif
