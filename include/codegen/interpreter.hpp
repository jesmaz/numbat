#pragma once


#include <codegen/targetVisitor.hpp>
#include <memory>
#include <nir/value.hpp>
#include <set>
#include <vector>


namespace codegen {

using namespace nir;

class Interpreter : public TargetVisitor {
	
	public:
		
		std::vector <Value> evaluate (const Instruction * val);
		
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
		virtual void visit (const Function * func);
		virtual void visit (const FunctionPointer & fptr);
		virtual void visit (const Get & get);
		virtual void visit (const Less & less);
		virtual void visit (const Mul & mul);
		virtual void visit (const Neg & neg);
		virtual void visit (const Number & num);
		virtual void visit (const Parameter & param);
		virtual void visit (const PtrAdd & ptrAdd);
		virtual void visit (const Put & put);
		virtual void visit (const Ret & ret);
		virtual void visit (const Struct & stru);
		virtual void visit (const Sub & sub);
		virtual void visit (const Type * type);
		
		virtual ~Interpreter ();
		
		
	protected:
	private:
		
		void cleanup ();
		
		Interpreter & operator = (const Interpreter & other)=default;
		
		virtual void generate (const Function * func, std::vector <uint8_t> & output) {}
		
		
		template <typename T>
		Value binaryOpp (Argument ilhs, Argument irhs, const Type * type);
		template <typename T>
		Value bitwiseOpp (Argument ilhs, Argument irhs, const Type * type);
		
		Value evaluate (Argument val);
		Value staticCast (const Value & source, const Type * const target);
		static std::vector <Value> callFunction (const Function * func, const std::vector <Value> & args);
		std::vector <Value> operator () (const Block * block);
		
		
		std::map <symbol, Value> lookupTable;
		const Block * currentBlock=nullptr;
		
};


};
