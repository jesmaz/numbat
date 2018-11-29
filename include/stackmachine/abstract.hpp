#pragma once


#include <stackmachine/chunk.hpp>
#include <utility/literals.hpp>


namespace stackmachine {


class AbstractStackMachine {
	
	public:
		
		static AbstractStackMachine & findTarget (const string & id);
		
		virtual void run (const Chunk & chunk, std::ostream & out)=0;
		
	protected:
	private:
		
};

template <typename Engine>
class GeneralStackMachine : public AbstractStackMachine {
	
	public:
		
		Engine & getEngine () {return e;}
		const Engine & getEngine () const {return e;}
		
		virtual void run (const Chunk & chunk, std::ostream & out) {
			
			e.section_layout (out);
			for (auto & pair : chunk.layouts) {
				e.layout (pair.first, pair.second, out);
			}
			
			e.section_data (out);
			for (auto & t : chunk.data) {
				e.data (std::get <0> (t), std::get <1> (t), std::get <2> (t), out);
			}
			
			e.section_text (out);
			for (auto & inst : chunk.text) {
				switch (inst.opcode) {
					case OP_CODE::CALL:
						e.op_call (inst, out);
						break;
					case OP_CODE::CALL_SYS:
						e.op_call_sys (inst, out);
						break;
					case OP_CODE::COPY:
						e.op_copy (inst, out);
						break;
					case OP_CODE::COPY_MEMCOPY:
						e.op_copy_memcopy (inst, out);
						break;
					case OP_CODE::DUPLICATE:
						e.op_duplicate (inst, out);
						break;
					case OP_CODE::JMP:
						e.op_jmp (inst, out);
						break;
					case OP_CODE::JMP_IF:
						e.op_jmp_if (inst, out);
						break;
					case OP_CODE::JMP_VAR:
						e.op_jmp_var (inst, out);
						break;
					case OP_CODE::LABEL:
						e.op_label (inst, out);
						break;
					case OP_CODE::LOAD:
						e.op_load (inst, out);
						break;
					case OP_CODE::LOAD_GLOBAL_ADDR:
						e.op_load_global_addr (inst, out);
						break;
					case OP_CODE::LOAD_INST_PTR:
						e.op_load_inst_ptr (inst, out);
						break;
					case OP_CODE::LOAD_STACK_ADDR:
						e.op_load_stack_addr (inst, out);
						break;
					case OP_CODE::MOVE:
						e.op_move (inst, out);
						break;
					case OP_CODE::OFFSET:
						e.op_offset (inst, out);
						break;
					case OP_CODE::OP_ADD:
						e.op_add (inst, out);
						break;
					case OP_CODE::OP_AND:
						e.op_and (inst, out);
						break;
					case OP_CODE::OP_CMP_EQ:
						e.op_cmp_eq (inst, out);
						break;
					case OP_CODE::OP_CMP_GT:
						e.op_cmp_gt (inst, out);
						break;
					case OP_CODE::OP_CMP_GTE:
						e.op_cmp_gte (inst, out);
						break;
					case OP_CODE::OP_CMP_LT:
						e.op_cmp_lt (inst, out);
						break;
					case OP_CODE::OP_CMP_LTE:
						e.op_cmp_lte (inst, out);
						break;
					case OP_CODE::OP_CMP_NE:
						e.op_cmp_ne (inst, out);
						break;
					case OP_CODE::OP_DIV:
						e.op_div (inst, out);
						break;
					case OP_CODE::OP_MUL:
						e.op_mul (inst, out);
						break;
					case OP_CODE::OP_NOT:
						e.op_not (inst, out);
						break;
					case OP_CODE::OP_OR:
						e.op_or (inst, out);
						break;
					case OP_CODE::OP_XOR:
						e.op_xor (inst, out);
						break;
					case OP_CODE::POP:
						e.op_pop (inst, out);
						break;
					case OP_CODE::RESERVE:
						e.op_reserve (inst, out);
						break;
					case OP_CODE::RET:
						e.op_ret (inst, out);
						break;
					case OP_CODE::SIZE_OF:
						e.op_size_of (inst, out);
						break;
					case OP_CODE::STORE_INST_PTR:
						e.op_store_inst_ptr (inst, out);
						break;
				}
			}
			
		}
		
	protected:
	private:
		
		Engine e;
		
};


}
