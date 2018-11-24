#pragma once


#include <stackmachine/instruction.hpp>
#include <utility/array.hpp>
#include <utility/symbol.hpp>


namespace stackmachine {


struct StackContentTracker {
	
	public:
		
		void addLayout (const symbol_t & sym, const BasicArray <TYPE> & layout) {layouts [sym] = layout;}
		int feed (const Instruction & inst) {
			int pos = stack.size ();
			switch (inst.opcode) {
				case OP_CODE::CALL: {
					auto & layout = layouts [inst.symbol];
					auto itt = layout.begin (), end = layout.end ();
					while (itt != end and *itt != TYPE::_META_FUNC_SPLIT) {
						stack.pop_back ();
						++itt;
					}
					stack.pop_back ();
					if (itt != end) ++itt;
					while (itt != end) {
						stack.push_back (*itt);
						++itt;
					}
					break;
				}
					
				case OP_CODE::COPY: {
					stack.pop_back (layouts [inst.symbol].size () + 1);
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
					stack.pop_back (3);
					break;
					
				case OP_CODE::LABEL:
					break;
					
				case OP_CODE::LOAD: {
					auto layout = layouts [inst.symbol];
					stack.pop_back ();
					for (auto t : layout) {
						stack.push_back (t);
					}
					break;
				}
				
				case OP_CODE::LOAD_GLOBAL_ADDR:
					stack.push_back (TYPE::usize);
					break;
					
				case OP_CODE::LOAD_INST_PTR:
					stack.push_back (TYPE::usize);
					break;
					
				case OP_CODE::LOAD_STACK_ADDR:
					stack.push_back (TYPE::usize);
					break;
					
				case OP_CODE::MOVE:
					stack.pop_back ();
					break;
					
				case OP_CODE::OFFSET:
					stack.push_back (TYPE::usize);
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
					stack.push_back (TYPE::u8);
					break;
					
				case OP_CODE::OP_DIV:
				case OP_CODE::OP_MUL:
					stack.pop_back ();
					break;
					
				case OP_CODE::OP_NOT:
					break;
					
				case OP_CODE::OP_OR:
				case OP_CODE::OP_XOR:
					stack.pop_back ();
					break;
					
				case OP_CODE::POP:
					stack.pop_back (inst.size);
					break;
					
				case OP_CODE::RESERVE: {
					auto layout = layouts [inst.symbol];
					for (auto t : layout) {
						stack.push_back (t);
					}
					break;
				}
					
				case OP_CODE::SIZE_OF:
					stack.push_back (TYPE::usize);
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
		
		DynArray <TYPE> stack;
		std::map <symbol_t, BasicArray <TYPE>> layouts;
		
};


}

