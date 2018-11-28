#pragma once


#include <ast/passes/shallnot.hpp>
#include <stackmachine/chunk.hpp>
#include <stackmachine/stackcontenttracker.hpp>
#include <utility/literals.hpp>


namespace AST {


class StackMachinePass : public ShallNotPass {
	
	public:
		
		struct Chunk {
			int variableSpace;
			DynArray <stackmachine::Instruction> inst;
			std::map <const Variable*, int> stackVariables;
			std::map <symbol_t, Literal> literals;
			std::map <symbol_t, std::pair <symbol_t, Literal>> globals;
			std::map <symbol_t, BasicArray <stackmachine::TYPE>> layouts;
			std::map <symbol_t, std::pair <BasicArray <symbol_t>, BasicArray <symbol_t>>> functionLayouts;
			stackmachine::StackContentTracker tracker;
			int push (const stackmachine::Instruction & i);
			symbol_t getLayout (const FuncPtr & type);
			symbol_t getLayout (const TypePtr & type);
		};
		
		virtual NodePtr visit (const NodePtr &) {abort ();}
		
		virtual void visit (const Basic_Operation & node);
		virtual void visit (const Call_0 & node);
		virtual void visit (const Call_1 & node);
		virtual void visit (const Call_2 & node);
		virtual void visit (const Call_n & node);
		virtual void visit (const Function_Ptr & node);
		virtual void visit (const IfElse & node);
		virtual void visit (const RawInit & node);
		virtual void visit (const Sequence & node);
		virtual void visit (const StaticIndex & node);
		virtual void visit (const StaticValue & node);
		virtual void visit (const SystemCall & node);
		virtual void visit (const Variable & node);
		
		StackMachinePass (Chunk & c) : c (c) {}
		
	protected:
		
		int load (const NodePtr & node);
		int push (const NodePtr & node);
		int push (const stackmachine::Instruction & i) {return c.push (i);}
		symbol_t getLayout (const FuncPtr & type) {return c.getLayout (type);}
		symbol_t getLayout (const TypePtr & type) {return c.getLayout (type);}
		
		Chunk & c;
		int size;
		
	private:
		
};

class StackMachineLoadPass : public StackMachinePass {
	
	public:
		
		virtual void visit (const Basic_Operation & node);
		virtual void visit (const RawInit & node);
		virtual void visit (const Sequence & node);
		virtual void visit (const StaticIndex & node);
		virtual void visit (const StaticValue & node);
		virtual void visit (const Variable & node);
		
		StackMachineLoadPass (Chunk & c) : StackMachinePass (c) {}
		
	protected:
	private:
		
};


stackmachine::Chunk stackMachinePass (FuncPtr func);
stackmachine::Chunk stackMachinePass (NodePtr body);


}
