#pragma once


#include <stackmachine/abstract.hpp>


namespace stackmachine {


struct PrintingEngine {
	
	public:
		
		void layout (const symbol_t & sym, const BasicArray <TYPE> & l, std::ostream & out) {
			out << "\t" << *sym;
			for (auto t : l) {
				switch (t) {
					case TYPE::i8:
						out << " i8";
						break;
					case TYPE::i16:
						out << " i16";
						break;
					case TYPE::i32:
						out << " i32";
						break;
					case TYPE::i64:
						out << " i64";
						break;
					case TYPE::isize:
						out << " isize";
						break;
					case TYPE::f16:
						out << " f16";
						break;
					case TYPE::f32:
						out << " f32";
						break;
					case TYPE::f64:
						out << " f64";
						break;
					case TYPE::u8:
						out << " u8";
						break;
					case TYPE::u16:
						out << " u16";
						break;
					case TYPE::u32:
						out << " u32";
						break;
					case TYPE::u64:
						out << " u64";
						break;
					case TYPE::usize:
						out << " usize";
						break;
					case TYPE::_META_FUNC_SPLIT:
						out << " ->";
						break;
					case TYPE::_META_STRUCT_BEGIN:
						out << " struct_beg";
						break;
					case TYPE::_META_STRUCT_END:
						out << " struct_end";
						break;
				}
			}
			out << "\n";
		}
		
		void data (const symbol_t & iden, const symbol_t & type, const Literal & literal, std::ostream & out) {
			out << "\t" << *iden << " " << *type << " " << literal.toString (text::PrintMode::PLAIN) << "\n";
		}
		
		void op_call (const Instruction & inst, std::ostream & out) {
			out << "\tcall " << *inst.symbol << "\n";
		}
		
		void op_call_sys (const Instruction & inst, std::ostream & out) {
			out << "\tcall_sys " << *inst.symbol << "\n";
		}
		
		void op_copy (const Instruction & inst, std::ostream & out) {
			out << "\tcopy " << *inst.symbol << "\n";
		}
		
		void op_copy_memcopy (const Instruction & inst, std::ostream & out) {
			out << "\tmemcpy\n";
		}
		
		void op_duplicate (const Instruction & inst, std::ostream & out) {
			out << "\tduplicate " << inst.size << "\n";
		}
		
		void op_jmp (const Instruction & inst, std::ostream & out) {
			out << "\tjmp " << *inst.symbol << "\n";
		}
		
		void op_jmp_if (const Instruction & inst, std::ostream & out) {
			out << "\tjmp_if " << *inst.symbol << "\n";
		}
		
		void op_jmp_var (const Instruction & inst, std::ostream & out) {
			out << "\tjmp_var " << *inst.symbol << "\n";
		}
		
		void op_label (const Instruction & inst, std::ostream & out) {
			out << *inst.symbol << "\n";
		}
		
		void op_load (const Instruction & inst, std::ostream & out) {
			out << "\tload " << *inst.symbol << "\n";
		}
		
		void op_load_global_addr (const Instruction & inst, std::ostream & out) {
			out << "\tload_global_addr " << *inst.symbol << "\n";
		}
		
		void op_load_inst_ptr (const Instruction & inst, std::ostream & out) {
			out << "\tload_inst_ptr\n";
		}
		
		void op_load_stack_addr (const Instruction & inst, std::ostream & out) {
			out << "\tload_stack_addr " << inst.size << "\n";
		}
		
		void op_move (const Instruction & inst, std::ostream & out) {
			out << "\tmove " << inst.size << "\n";
		}
		
		void op_offset (const Instruction & inst, std::ostream & out) {
			out << "\toffset " << inst.size << " " << *inst.symbol << "\n";
		}
		
		void op_add (const Instruction & inst, std::ostream & out) {
			out << "\tadd\n";
		}
		
		void op_and (const Instruction & inst, std::ostream & out) {
			out << "\tand\n";
		}
		
		void op_cmp_eq (const Instruction & inst, std::ostream & out) {
			out << "\tcmp_eq\n";
		}
		
		void op_cmp_gt (const Instruction & inst, std::ostream & out) {
			out << "\tcmp_gt\n";
		}
		
		void op_cmp_gte (const Instruction & inst, std::ostream & out) {
			out << "\tcmp_gte\n";
		}
		
		void op_cmp_lt (const Instruction & inst, std::ostream & out) {
			out << "\tcmp_lt\n";
		}
		
		void op_cmp_lte (const Instruction & inst, std::ostream & out) {
			out << "\tcmp_lte\n";
		}
		
		void op_cmp_ne (const Instruction & inst, std::ostream & out) {
			out << "\tcmp_ne\n";
		}
		
		void op_div (const Instruction & inst, std::ostream & out) {
			out << "\tdiv\n";
		}
		
		void op_mul (const Instruction & inst, std::ostream & out) {
			out << "\tmul\n";
		}
		
		void op_not (const Instruction & inst, std::ostream & out) {
			out << "\tnot\n";
		}
		
		void op_or (const Instruction & inst, std::ostream & out) {
			out << "\tor\n";
		}
		
		void op_sub (const Instruction & inst, std::ostream & out) {
			out << "\tsub\n";
		}
		
		void op_xor (const Instruction & inst, std::ostream & out) {
			out << "\txor\n";
		}
		
		void op_pop (const Instruction & inst, std::ostream & out) {
			out << "\tpop " << inst.size << "\n";
		}
		
		void op_reserve (const Instruction & inst, std::ostream & out) {
			out << "\treserve " << *inst.symbol << "\n";
		}
		
		void op_size_of (const Instruction & inst, std::ostream & out) {
			out << "\treserve " << *inst.symbol << "\n";
		}
		
		void op_store_inst_ptr (const Instruction & inst, std::ostream & out) {
			out << "\tstore_inst_ptr\n";
		}
		
		void section_data (std::ostream & out) {
			out << ".data\n";
		}
		
		void section_layout (std::ostream & out) {
			out << ".layouts\n";
		}
		
		void section_text (std::ostream & out) {
			out << ".text\n";
		}
		
	protected:
	private:
		
		
		
};

typedef GeneralStackMachine <PrintingEngine> StackPrinter;


}

