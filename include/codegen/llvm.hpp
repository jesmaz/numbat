#ifdef USE_LLVM
#pragma once


#include <codegen/targetVisitor.hpp>
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/IRBuilder.h>

#include <map>


namespace codegen {

using namespace nir;
using numbat::visitor::ConstVisitor;

class LLVM : public TargetVisitor {
	
	public:
		
		virtual void finalise ();
		
		virtual void reset ();
		
		virtual void visit (const Add & add);
		virtual void visit (const Alloc & alloc);
		virtual void visit (const BitAnd & bitAnd);
		virtual void visit (const BitNot & bitNot);
		virtual void visit (const BitOr & bitOr);
		virtual void visit (const BitXor & bitXor);
		virtual void visit (const Block & block);
		virtual void visit (const Composite & comp);
		virtual void visit (const Constant & con);
		virtual void visit (const DirectCall & call);
		virtual void visit (const Div & div);
		virtual void visit (const Function * func);
		virtual void visit (const FunctionPointer & fPtr) {}
		virtual void visit (const Get & get) {}
		virtual void visit (const Less & less) {}
		virtual void visit (const Mul & mul);
		virtual void visit (const Neg & neg);
		virtual void visit (const Number & num);
		virtual void visit (const Parameter & param) {}
		virtual void visit (const PtrAdd & ptrAdd);
		virtual void visit (const Put & put);
		virtual void visit (const Ret & ret);
		virtual void visit (const Struct & stru);
		virtual void visit (const Sub & sub);
		virtual void visit (const Type * type);
		
		LLVM ();
		
	protected:
	private:
		
		llvm::Type * resolve (const Type * type);
		llvm::Function * resolve (const Function * func);
		llvm::Value * resolve (nir::Argument val);
		
		virtual void generate (const Function * func, std::vector <uint8_t> & output) {}
		
		llvm::Module * module;
		llvm::DIBuilder diBuilder;
		llvm::IRBuilder<> irBuilder;
		
		std::map <const Function *, llvm::Function *> funcDict;
		std::map <symbol, llvm::Value *> instrDict;
		std::map <const nir::Type *, llvm::Type *> typeDict;
		
		static Target::RegTarget target;
		
};


};

#endif
