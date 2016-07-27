#ifdef USE_LLVM


#include <codegen/llvm.hpp>
#include <iostream>
#include <llvm/Analysis/Passes.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/IR/Attributes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/PassManager.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/ToolOutputFile.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetLibraryInfo.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/Scalar.h>
#include <nir/block.hpp>
#include <nir/function.hpp>
#include <nir/type/number.hpp>
#include <utility/config.hpp>


namespace codegen {


Target::RegTarget LLVM::target ("llvm", []()->Target*{
	llvm::InitializeAllTargets ();
	llvm::InitializeAllTargetMCs ();
	llvm::InitializeAllAsmPrinters ();
	llvm::InitializeAllAsmParsers ();
	llvm::InitializeNativeTarget ();
	return new LLVM;
});


llvm::Type * LLVM::resolve (const Type * type) {
	assert (type);
	auto * t = typeDict [type];
	if (not t) {
		type->accept (*this);
		t = typeDict [type];
	}
	assert (t);
	return t;
	
}

llvm::Function * LLVM::resolve (const nir::Function * func) {
	
	assert (func);
	auto & f = funcDict [func];
	
	if (f) return f;
	
	std::vector <llvm::Type *> params, ret;
	for (const Parameter * param : func->getArgs ()) {
		params.push_back (resolve (param->getType ()));
	}
	
	for (const Type * t : func->getRetTypes ()) {
		ret.push_back (resolve (t));
	}
	
	llvm::Type * retType;
	if (ret.empty ()) {
		retType = irBuilder.getVoidTy ();
	} else {
		retType = llvm::StructType::get (irBuilder.getContext (), ret);
	}
	
	llvm::FunctionType * ft = llvm::FunctionType::get (retType, params, false);
	return f = llvm::Function::Create (ft, llvm::Function::ExternalLinkage, func->getLabel (), module);
	
}

llvm::Value * LLVM::resolve (nir::Argument val) {
	auto & v = instrDict [val.sym];
	if (not val.sym or not v) {
		val.instr->accept (*this);
		assert (v);
	}
	return v;
}

void LLVM::finalise () {
	
	const Config & config = Config::globalConfig ();
	
	std::string error;
	
	llvm::Triple theTriple (module->getTargetTriple ());
	const llvm::Target * target = llvm::TargetRegistry::lookupTarget ("x86-64", theTriple, error);
	llvm::TargetRegistry::printRegisteredTargetsForVersion ();
	
	if (not target) {
		std::cerr << error << std::endl;
		std::cerr << theTriple.getTriple () << std::endl;
		return;
	}
	
	llvm::TargetOptions options;
	llvm::TargetMachine * machine = target->createTargetMachine (theTriple.getTriple (), "generic", "", options);
	
	if (not machine) {
		std::cerr << "could not allocate target machine" << std::endl;
		return;
	}
	
	
	llvm::tool_output_file fout ((config.outfile + ".o").c_str (), error, llvm::sys::fs::F_None);
	if (!error.empty ()) {
		std::cerr << error << std::endl;
		return;
	}
	
	
	llvm::PassManager pm;
	pm.add (new llvm::TargetLibraryInfo (theTriple));
	machine->addAnalysisPasses (pm);
	pm.add (new llvm::DataLayoutPass (*machine->getDataLayout ()));
	llvm::formatted_raw_ostream frouts (fout.os ());
	machine->addPassesToEmitFile (pm, frouts, llvm::TargetMachine::CGFT_ObjectFile);
	
	pm.run (*module);
	fout.keep ();
	
}

void LLVM::reset () {
	delete module;
	new (this) LLVM;
}


void LLVM::visit (const nir::Add & add) {
	//auto * type = resolve (add.getType ())
	auto * lhs = resolve (add.getLhs ()/*, type*/), * rhs = resolve (add.getRhs ()/*, type*/);
	instrDict [add.getIden ()] = irBuilder.CreateAdd (lhs, rhs, "add");
}

void LLVM::visit (const nir::Alloc & alloc) {
	auto * arraySize = resolve (alloc.getAmount ());
	instrDict [alloc.getIden ()] = irBuilder.CreateAlloca (resolve (alloc.getType ()->getDereferenceType ()), arraySize);
}

void LLVM::visit (const nir::BitAnd & bitAnd) {
	auto * lhs = resolve (bitAnd.getLhs ()/*, type*/), * rhs = resolve (bitAnd.getRhs ()/*, type*/);
	instrDict [bitAnd.getIden ()] = irBuilder.CreateAdd (lhs, rhs, "and");
}

void LLVM::visit (const nir::BitNot & bitNot) {
	auto * arg = resolve (bitNot.getArg ()/*, type*/);
	instrDict [bitNot.getIden ()] = irBuilder.CreateNot (arg, "not");
}

void LLVM::visit (const nir::BitOr & bitOr) {
	auto * lhs = resolve (bitOr.getLhs ()/*, type*/), * rhs = resolve (bitOr.getRhs ()/*, type*/);
	instrDict [bitOr.getIden ()] = irBuilder.CreateOr (lhs, rhs, "or");
}

void LLVM::visit (const nir::BitXor & bitXor) {
	auto * lhs = resolve (bitXor.getLhs ()/*, type*/), * rhs = resolve (bitXor.getRhs ()/*, type*/);
	instrDict [bitXor.getIden ()] = irBuilder.CreateXor (lhs, rhs, "xor");
}

void LLVM::visit (const nir::Block & block) {
	//TODO: Make insertion block relevant
	std::cerr << "\nBlock\n\n";
	for (const nir::Instruction * val : block.getInstructions ()) {
		std::cerr << val->toString () << " " << typeid (*val).name () << " " << (void*)val << std::endl;
		val->accept (*this);
		if (not val->getIdens ().empty ()) {
			instrDict [val->getIden ()]->dump ();
		}
	}
}

void LLVM::visit (const nir::Constant & con) {
	
	llvm::Value * val;
	
	const auto & syms = con.getIdens ();
	const auto & data = con.getData ();
	const auto & types = con.getTypes ();
	
	assert (syms.size () == data.size ());
	
	for (size_t i=0, l=syms.size (); i<l; ++i) {
		
		const Constant::Data & d = *data [i];
		
		switch (d.getType ()) {
			case Constant::Data::DOUBLE:
				val = llvm::ConstantFP::get (irBuilder.getDoubleTy (), d.asDouble ());
				break;
			case Constant::Data::INT:
				val = irBuilder.getIntN (64, d.asUint ());
				break;
			case Constant::Data::STRING:
				break;
			case Constant::Data::UINT:
				val = irBuilder.getIntN (64, d.asUint ());
				break;
		}
		
		instrDict [syms [i]] = val;
		
	}
	
}

void LLVM::visit (const nir::DirectCall & call) {
	
	std::vector <llvm::Value *> args;
	for (auto a : call.getArgs ()) {
		args.push_back (resolve (a));
	}
	irBuilder.CreateCall (resolve (call.getFunc ()), args);
	
}

void LLVM::visit (const nir::Div & div) {
	auto * lhs = resolve (div.getLhs ()), * rhs = resolve (div.getRhs ());
	instrDict [div.getIden ()] = irBuilder.CreateMul (lhs, rhs, "mul");
}

void LLVM::visit (const nir::Function * func) {
	
	//TODO: Pre create blocks
	//TODO: Give function arguments and a return type
	auto * llFunc = resolve (func);
	if (not llFunc->empty ()) return;
	std::cerr << "\nVisiting " << std::hex << (void*) func << std::endl;
	llFunc->setCallingConv (llvm::CallingConv::C);
	funcDict [func] = llFunc;
	//TODO: Make insertion blocks consistent
	irBuilder.SetInsertPoint (llvm::BasicBlock::Create (irBuilder.getContext (), "entry", llFunc));
	
	auto argItt = llFunc->arg_begin ();
	auto argEnd = llFunc->arg_end ();
	for (const Parameter * param : func->getArgs ()) {
		assert (argItt != argEnd);
		instrDict [param->getIden ()] = argItt;
		++argItt;
	}
	
	visit (*func->getEntryPoint ());
	
	const auto & retTypes = func->getRetTypes ();
	if (retTypes.empty ()) {
		irBuilder.CreateRetVoid ();
	} else {
		//TODO: return last instruction
	}
	
	instrDict.clear ();
	
	std::cerr << std::flush;
	llvm::raw_fd_ostream llerr (2, false);
	if (llvm::verifyFunction (*llFunc, &llerr)) {
		llFunc->print (llerr);
		llerr.flush ();
	}
	
}

void LLVM::visit (const nir::Mul & mul) {
	auto * lhs = resolve (mul.getLhs ()), * rhs = resolve (mul.getRhs ());
	instrDict [mul.getIden ()] = irBuilder.CreateMul (lhs, rhs, "mul");
}

void LLVM::visit (const nir::Neg & neg) {
	auto * arg = resolve (neg.getArg ());
	if (arg->getType ()->isFloatingPointTy ()) {
		instrDict [neg.getIden ()] = irBuilder.CreateFNeg (arg, "neg");
	} else {
		instrDict [neg.getIden ()] = irBuilder.CreateNeg (arg, "neg");
	}
}

void LLVM::visit (const nir::Number & num) {
	switch (num.getArithmaticType ()) {
		case Type::DECINT:
		case Type::DEFAULT:
			assert (false);
		case Type::FPINT:
			typeDict [&num] = irBuilder.getDoubleTy ();
			break;
		case Type::INT:
		case Type::UINT:
			typeDict [&num] = irBuilder.getIntNTy (64);
			break;
	}
}

void LLVM::visit (const nir::Put & put) {
	auto * src = resolve (put.getSrc ());
	auto * dest = resolve (put.getDest ());
	instrDict [put.getIden ()] = irBuilder.CreateStore (src, dest);
}

void LLVM::visit (const nir::Ret & ret) {
	if (ret.getArgs ().empty ()) {
		irBuilder.CreateRetVoid ();
	} else {
		std::vector <llvm::Value *> args;
		for (auto a : ret.getArgs ()) {
			args.push_back (resolve (a));
		}
		irBuilder.CreateAggregateRet (args.data (), args.size ());
	}
}

void LLVM::visit (const nir::Struct & stru) {
	
	std::vector <llvm::Type *> members;
	members.reserve (stru.getMemberArr ().size ());
	for (const nir::Parameter * p : stru.getMemberArr ()) {
		members.push_back (resolve (p->getType ()));
	}
	typeDict [&stru] = llvm::StructType::get (irBuilder.getContext (), members);
	
}

void LLVM::visit (const nir::Sub & sub) {
	//auto * type = resolve (add.getType ())
	auto * lhs = resolve (sub.getLhs ()/*, type*/), * rhs = resolve (sub.getRhs ()/*, type*/);
	instrDict [sub.getIden ()] = irBuilder.CreateSub (lhs, rhs, "sub");
}

void LLVM::visit (const Type * type) {

}

LLVM::LLVM () : module (new llvm::Module ("numbat", llvm::getGlobalContext ())), diBuilder (*module), irBuilder (llvm::getGlobalContext ()) {
	
}


};


#endif