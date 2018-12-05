#pragma once


#include <stackmachine/instruction.hpp>
#include <utility/array.hpp>
#include <utility/symbol.hpp>


namespace stackmachine {


struct StackContentTracker {
	
	public:
		
		void addLayout (const symbol_t & sym, const BasicArray <TYPE> & layout) {layouts [sym] = layout;}
		void addLayout (const symbol_t & sym, const std::pair <BasicArray <symbol_t>, BasicArray <symbol_t>> & layout) {functionLayouts [sym] = layout;}
		int feed (const Instruction & inst) {
			int pos = stack.size ();
			switch (inst.opcode) {
				case OP_CODE::CALL: {
					auto itt = functionLayouts.find (inst.symbol);
					assert (itt != functionLayouts.end ());
					auto & layout = itt->second;
					for (size_t i=0; i<layout.first.size (); ++i) {
						stack.pop_back ();
					}
					stack.pop_back ();
					for (size_t i=0; i<layout.second.size (); ++i) {
						stack.push_back (layout.second [i]);
					}
					break;
				}
				case OP_CODE::CALL_SYS: {
					if (*inst.symbol == "close") {
						//int close (int)
						//net result: no stack change
					} else if (*inst.symbol == "exit") {
						//Exit doesn't actually return, but we pretend it returns an int
					} else if (*inst.symbol == "open") {
						//int open (const char *, int, int)
						stack.pop_back (3);
						stack.push_back (symbol_t ("uint0"));
					} else if (*inst.symbol == "read") {
						//ssize_t read (int, void *, size_t)
						stack.pop_back (3);
						stack.push_back (symbol_t ("uint0"));
					} else {
						abort ();
					}
					break;
				}
				
				case OP_CODE::CONVERT: {
					stack.back () = inst.symbol;
					break;
				}
				
				case OP_CODE::COPY: {
					stack.pop_back (2);
					break;
				}
				
				case OP_CODE::COPY_MEMCOPY:
					stack.pop_back (3);
					break;
					
				case OP_CODE::DUPLICATE:
					if (inst.size >= 0) {
						stack.push_back (stack [inst.size]);
					} else {
						stack.push_back (stack [stack.size () + inst.size]);
					}
					break;
					
				case OP_CODE::JMP:
					break;
					
				case OP_CODE::JMP_IF:
					stack.pop_back ();
					break;
					
				case OP_CODE::JMP_VAR:
					stack.pop_back ();
					break;
					
				case OP_CODE::LABEL:
					break;
					
				case OP_CODE::LOAD: {
					stack.pop_back ();
					stack.push_back (inst.symbol);
					break;
				}
				
				case OP_CODE::LOAD_GLOBAL_ADDR:
					stack.push_back (inst.symbol);
					break;
					
				case OP_CODE::LOAD_INST_PTR:
					stack.push_back (symbol_t ("uint0"));
					break;
					
				case OP_CODE::LOAD_STACK_ADDR:
					stack.push_back (symbol_t ("uint0"));
					break;
					
				case OP_CODE::MOVE:
					stack.pop_back ();
					break;
					
				case OP_CODE::OFFSET:
					stack.pop_back ();
					stack.push_back (inst.symbol);
					break;
					
				case OP_CODE::OP_ADD:
				case OP_CODE::OP_AND:
					stack.pop_back ();
					break;
					
				case OP_CODE::OP_CMP_EQ:
				case OP_CODE::OP_CMP_GT:
				case OP_CODE::OP_CMP_GTE:
				case OP_CODE::OP_CMP_LT:
				case OP_CODE::OP_CMP_LTE:
				case OP_CODE::OP_CMP_NE:
					stack.pop_back (2);
					stack.push_back (symbol_t ("uint1"));
					break;
					
				case OP_CODE::OP_DIV:
				case OP_CODE::OP_MUL:
					stack.pop_back ();
					break;
					
				case OP_CODE::OP_NOT:
					break;
					
				case OP_CODE::OP_OR:
				case OP_CODE::OP_SUB:
				case OP_CODE::OP_XOR:
					stack.pop_back ();
					break;
					
				case OP_CODE::POP:
					stack.pop_back (inst.size);
					break;
					
				case OP_CODE::RESERVE: {
					stack.push_back (inst.symbol);
					break;
				}
				
				case OP_CODE::RET:
					//A return will not actually pop data in practice
					//we just pretend it does
					stack.pop_back (inst.size);
					break;
				
				case OP_CODE::SIZE_OF:
					stack.push_back (symbol_t ("uint0"));
					break;
					
				case OP_CODE::STORE_INST_PTR:
					stack.pop_back ();
					break;
			}
			return pos;
		}
		size_t getStackSize () const {return stack.size ();}
		
	protected:
	private:
		
		
		
		DynArray <symbol_t> stack;
		std::map <symbol_t, BasicArray <TYPE>> layouts;
		std::map <symbol_t, std::pair <BasicArray <symbol_t>, BasicArray <symbol_t>>> functionLayouts;
		
};


}

