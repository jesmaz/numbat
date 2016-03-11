#pragma once

#include "../nir/inst/add.hpp"
#include "../nir/inst/alloc.hpp"
#include "../nir/inst/call.hpp"
#include "../nir/inst/constant.hpp"
#include "../nir/inst/functionPointer.hpp"
#include "../nir/inst/get.hpp"
#include "../nir/inst/mul.hpp"
#include "../nir/inst/put.hpp"
#include "../nir/inst/sub.hpp"
#include "../nir/parameter.hpp"

#include "target.hpp"

#include <memory>
#include <set>
#include <vector>


namespace codegen {

using namespace nir;
using numbat::visitor::ConstVisitor;

class Interpreter : public Target, public ConstVisitor <Add>, public ConstVisitor <Alloc>, public ConstVisitor <Constant>, public ConstVisitor <DirectCall>, public ConstVisitor <FunctionPointer>, public ConstVisitor <Get>, public ConstVisitor <Mul>, public ConstVisitor <Number>, public ConstVisitor <Parameter>, public ConstVisitor <Put>, public ConstVisitor <Sub> {
	
	public:
		
		virtual void reset ();
		
		virtual void visit (const Add & add);
		virtual void visit (const Alloc & alloc);
		virtual void visit (const Block & block);
		virtual void visit (const Constant & con);
		virtual void visit (const DirectCall & call);
		virtual void visit (const Function * func);
		virtual void visit (const FunctionPointer & fptr);
		virtual void visit (const Get & get);
		virtual void visit (const Mul & mul);
		virtual void visit (const Number & num);
		virtual void visit (const Parameter & param);
		virtual void visit (const Put & put);
		virtual void visit (const Sub & sub);
		virtual void visit (const Type * type);
		
		string operator () (const Instruction * val);
		
		virtual ~Interpreter ();
		
	protected:
	private:
		
		void cleanup ();
		
		Interpreter & operator = (const Interpreter & other)=default;
		
		virtual void generate (const Function * func, std::vector <uint8_t> & output) {}
		
		enum AtomicType {DATA, F32, F64, FUNCTION, PTR, S8, S16, S32, S64, STRUCT, U8, U16, U32, U64, VOID};
		
		union AtomicData {
			double f64;
			float f32;
			int8_t s8;
			int16_t s16;
			int32_t s32;
			int64_t s64;
			uint8_t u8;
			uint16_t u16;
			uint32_t u32;
			uint64_t u64;
			uint8_t * ptr;
			const Function * func;
		};
		
		struct Atom {
			AtomicType atomicType;
			AtomicData data;
			const Type * type;
		};
		
		template <typename T>
		Interpreter::Atom binaryOpp (const Instruction * ilhs, const Instruction * irhs, const Type * type);
		
		Atom lookupAtom (const Instruction * val);
		Atom staticCast (const Atom & source, const Type * const target);
		static std::vector <Atom> callFunction (const Function * func, const std::vector <Atom> & args);
		std::vector <Atom> operator () (const Block * block);
		
		std::map <symbol, Atom> lookupTable;
		std::set <void *> ptrs;
		const Block * currentBlock=nullptr;
		
};


};