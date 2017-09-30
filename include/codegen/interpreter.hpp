#pragma once


#include <codegen/targetVisitor.hpp>
#include <memory>
#include <nir/block.hpp>
#include <nir/value.hpp>
#include <set>


namespace codegen {

using namespace nir;

class Interpreter : public TargetVisitor {
	
	public:
		
		string operator () (const Instruction * val);
		
		virtual void reset ();
		
		virtual void visit (const Add & add);
		virtual void visit (const Alloc & alloc);
		virtual void visit (const BitAnd & bitAnd);
		virtual void visit (const BitNot & bitNot);
		virtual void visit (const BitOr & bitOr);
		virtual void visit (const BitXor & bitXor);
		virtual void visit (const Block & block);
		virtual void visit (const Composite & com);
		virtual void visit (const Constant & con);
		virtual void visit (const Div & div);
		virtual void visit (const DirectCall & call);
		virtual void visit (const Equal & equal);
		virtual void visit (const Function * func);
		virtual void visit (const FunctionPointer & fptr);
		virtual void visit (const Get & get);
		virtual void visit (const Jump & jump);
		virtual void visit (const Less & less);
		virtual void visit (const LessEqual & lessequal);
		virtual void visit (const Mul & mul);
		virtual void visit (const Neg & neg);
		virtual void visit (const NEqual & nequal);
		virtual void visit (const Number & num);
		virtual void visit (const Parameter & param);
		virtual void visit (const PtrAdd & ptrAdd);
		virtual void visit (const Put & put);
		virtual void visit (const Ret & ret);
		virtual void visit (const Reinterpret & reinterpret);
		virtual void visit (const Tuple & tuple);
		virtual void visit (const Sub & sub);
		virtual void visit (const Type * type);
		
		
		Interpreter (const Block * entry) : entry (entry), instItt (entry->beg ()) {}
		virtual ~Interpreter ();
		
		
	protected:
		
		BasicArray <Value> evaluate (const Instruction * val);
		
	private:
		
		void cleanup ();
		
		Interpreter & operator = (const Interpreter & other)=default;
		
		virtual void generate (const Function * func, BasicArray <uint8_t> & output) {}
		
		
		template <typename T>
		Value binaryOpp (Argument ilhs, Argument irhs, const Type * type);
		template <typename T>
		Value bitwiseOpp (Argument ilhs, Argument irhs, const Type * type);
		
		Value evaluate (Argument val);
		Value staticCast (const Value & source, const Type * const target);
		static BasicArray <Value> callFunction (const Function * func, const BasicArray <Value> & args);
		BasicArray <Value> operator () ();
		
		
		std::map <symbol, Value> lookupTable;
		const Block * entry;
		Block::Iterator instItt;
		
};


};
