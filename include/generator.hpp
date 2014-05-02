#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <llvm/Analysis/Passes.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/IR/Attributes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
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


class BodyGenerator : public Visitor <ASTnumbatInstr>, public Visitor <ASTbody>, public Visitor <ASTcall>, public Visitor <ASTcallindex>, public Visitor <ASTconstantInt>, public Visitor <ASTconstantCString>, public Visitor <ASTfunctionPointer>, public Visitor <ASTparamater>, public Visitor <ASTrawdata>, public Visitor <ASTreturn>, public Visitor <ASTreturnvoid>, public Visitor <ASTstructIndex>, public Visitor <ASTtuplecall>, public Visitor <ASTtype>, public Visitor <ASTvariable>, public Visitor <ASTwhileloop> {
	public:
		virtual void visit (AbstractSyntaxTree & exp);
		virtual void visit (ASTbody & exp);
		virtual void visit (ASTcall & exp);
		virtual void visit (ASTcallindex & exp);
		virtual void visit (ASTconstantInt & exp);
		virtual void visit (ASTconstantCString & exp);
		virtual void visit (ASTfunctionPointer & exp) {}
		virtual void visit (ASTnumbatInstr & exp);
		virtual void visit (ASTparamater & exp);
		virtual void visit (ASTrawdata & exp) {}
		virtual void visit (ASTreturn & exp);
		virtual void visit (ASTreturnvoid & exp);
		virtual void visit (ASTstructIndex & exp);
		virtual void visit (ASTtuplecall & exp);
		virtual void visit (ASTtype & exp) {}
		virtual void visit (ASTvariable & exp);
		virtual void visit (ASTwhileloop & exp);
		void visit (const shared_ptr <Module> & module);
		//std::vector <Value *> operator () (AbstractSyntaxTree & ast) {tree = &ast; ast.getBody ().accept (*this); return body;}
		BodyGenerator (llvm::Module * mod, const DataLayout * dataLayout, FunctionPassManager * fpm=nullptr) : breakBlock (nullptr), continueBlock (nullptr), dataLayout (dataLayout), activeFunction (nullptr), memalloc (nullptr), memfree (nullptr), builder (getGlobalContext ()), context (getGlobalContext ()), module (mod), fpm (fpm) {}
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
		
		Type * getType (const ASTnode & node);
		Type * getType (const NumbatType * type);
		Value * getVariableHandle (const NumbatVariable * var);
		void makeCompare (const ASTnode & exp);
		void registerFunction (const FunctionDecleration * func);
		
		AbstractSyntaxTree * tree;
		BasicBlock * breakBlock, * continueBlock;
		const DataLayout * dataLayout;
		Function * activeFunction, * memalloc, * memfree;
		FunctionDecleration * activeFunctionDecleration;
		IRBuilder<> builder;
		LLVMContext & context;
		llvm::Module * module;
		FunctionPassManager * fpm;
		std::map <const ASTbase *, Value *> tempValues;
		std::map <const FunctionDecleration *, Function *> functions;
		std::map <const NumbatType *, Type *> structures;
		std::map <const NumbatVariable *, Value *> namedValues;
		//std::stack <stackVal> stack;
		std::set <Module *> builtModules;
		std::stack <Value *> stack;
		std::vector <Value *> body;
		//CallMeta callmeta;
		bool ref = false;
};


};
};


#endif
