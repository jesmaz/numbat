#pragma once


#include <fcntl.h>
#include <map>
#include <memory>
#include <stackmachine/abstract.hpp>
#include <stackmachine/printer.hpp>
#include <unistd.h>
#include <utility/literals.hpp>


namespace stackmachine {


	struct InterpretingStackMachine : public AbstractStackMachine  {
	
	public:
		
		int getExitCode () {return exitCode;}
		
		Literal getTop () {
			if (stackPos) {
				auto layout = stackDataLayout [dataLayoutPos - 1];
				return layout.dataToLiteral (&stack [stackPos - layout.getSize ()]);
			} else {
				abort ();
			}
		}
		
		Literal popTop () {
			if (stackPos) {
				auto layout = stackDataLayout [--dataLayoutPos];
				return layout.dataToLiteral (&stack [stackPos -= layout.getSize ()]);
			} else {
				abort ();
			}
		}
		
		void layout (const symbol_t & sym, const BasicArray <TYPE> & l, std::ostream &) {
			layouts [sym] = l;
		}
		
		void data (const symbol_t & iden, const symbol_t & type, const Literal & literal, std::ostream & out) {
			auto layout = layouts [type];
			uint8_t * data = new uint8_t [layout.getSize ()];
			layout.literalToData (literal, data, true);
			globals [iden] = std::unique_ptr <uint8_t> (data);
		}
		
		void op_call (const Instruction & inst, const Chunk & chunk, std::ostream & out) {
			
			auto itt = functionLayouts.find (inst.symbol);
			if (itt == functionLayouts.end ()) {
				abort ();
			}
			
			auto func = popTop ();
			auto symItt = symbolTable.find (inst.symbol);
			
			if (symItt != symbolTable.end ()) {
				
				dataLayoutPos -= itt->second.first.size ();
				for (size_t i=dataLayoutPos, l=dataLayoutPos+itt->second.first.size (); i<l; ++i) {
					stackPos -= stackDataLayout [i].getSize ();
				}
				
				auto resume = textRegister;
				auto oldFrame = stackFrame;
				auto oldLayoutFrame = layoutFrame;
				layoutFrame = dataLayoutPos;
				stackFrame = stackPos;
				
				runFrom (chunk, out, symItt->second);
				
				dataLayoutPos = layoutFrame;
				stackPos = stackFrame;
				
				dataLayoutPos += itt->second.second.size ();
				for (size_t i=layoutFrame, l=dataLayoutPos; i<l; ++i) {
					stackPos += stackDataLayout [i].getSize ();
				}
				
				stackFrame = oldFrame;
				layoutFrame = oldLayoutFrame;
				
				if (textRegister < textLength + 1) {
					textRegister = resume;
				}
				
			} else {
				abort ();
			}
			
		}
		
		void op_call_sys (const Instruction & inst, std::ostream & out) {
			
			if (*inst.symbol == "close") {
				auto fd = popTop ().to_int64 ();
				auto e = close (fd);
				Layout lay (TYPE::u32);
				stackDataLayout [dataLayoutPos++] = lay;
				lay.literalToData (e, &(stack [stackPos]));
				stackPos += lay.getSize ();
				
			} else if (*inst.symbol == "exit") {
				textRegister = textLength + 1;
				exitCode = popTop ().to_int64 ();
				
			} else if (*inst.symbol == "open") {
				auto mode = popTop ().to_int64 ();
				auto flags = popTop ().to_int64 ();
				auto name = (char *) (size_t (popTop ().to_uint64 ()));
				auto fd = open (name, flags, mode);
				perror (name);
				std::cerr << std::dec << size_t (name) << std::endl;
				Layout lay (TYPE::u32);
				stackDataLayout [dataLayoutPos++] = lay;
				lay.literalToData (fd, &(stack [stackPos]));
				stackPos += lay.getSize ();
				
			} else {
				abort ();
			}
			
		}
		
		void op_copy (const Instruction & inst, std::ostream & out) {
			
			auto & dest = stack [stackPos--];
			
		}
		
		void op_copy_memcopy (const Instruction & inst, std::ostream & out) {
			abort ();
			out << "\tmemcpy\n";
		}
		
		void op_duplicate (const Instruction & inst, std::ostream & out) {
			if (inst.size < 0) {
				size_t offset = 0;
				for (int i=inst.size; i<0; ++i) {
					offset += stackDataLayout [dataLayoutPos + i].getSize ();
				}
				auto & layout = stackDataLayout [dataLayoutPos+inst.size];
				stackDataLayout [dataLayoutPos++] = layout;
				std::copy (&(stack [stackPos - offset]), &(stack [stackPos - offset + layout.getSize ()]), &(stack [stackPos]));
				stackPos += layout.getSize ();
			} else {
				size_t offset = 0;
				for (int i=0; i+1<inst.size; ++i) {
					offset += stackDataLayout [layoutFrame + i].getSize ();
				}
				auto & layout = stackDataLayout [layoutFrame+inst.size];
				stackDataLayout [dataLayoutPos++] = layout;
				std::copy (&(stack [stackFrame + offset]), &(stack [stackFrame + offset + layout.getSize ()]), &(stack [stackPos]));
				stackPos += layout.getSize ();
			}
		}
		
		void op_jmp (const Instruction & inst, std::ostream & out) {
			auto symItt = symbolTable.find (inst.symbol);
			assert (symItt != symbolTable.end ());
			textRegister = symItt->second;
		}
		
		void op_jmp_if (const Instruction & inst, std::ostream & out) {
			auto val = popTop ();
			if (val != 0) {
				op_jmp (inst, out);
			}
			out << val.toString (text::PrintMode::PLAIN) << std::endl;
		}
		
		void op_jmp_var (const Instruction & inst, std::ostream & out) {
			abort ();
			out << "\tjmp_var " << *inst.symbol << "\n";
		}
		
		void op_label (const Instruction & inst, std::ostream & out) {}
		
		void op_load (const Instruction & inst, std::ostream & out) {
			auto layout = layouts [inst.symbol];
			stackPos -= sizeof (size_t);
			--dataLayoutPos;
			assert (stackDataLayout [dataLayoutPos].getSize () == sizeof (size_t));
			uint8_t * src = *reinterpret_cast <uint8_t**> (&(stack [stackPos]));
			uint8_t * dest = &(stack [stackPos]);
			size_t size = layout.getSize ();
			std::copy (src, src + size, dest);
			stackPos += size;
			stackDataLayout [dataLayoutPos++] = layout;
		}
		
		void op_load_global_addr (const Instruction & inst, std::ostream & out) {
			*reinterpret_cast <size_t*> (&(stack [stackPos])) = size_t (globals [inst.symbol].get ());
			stackPos += sizeof (size_t);
			stackDataLayout [dataLayoutPos++] = Layout (TYPE::usize);
		}
		
		void op_load_inst_ptr (const Instruction &, std::ostream &) {
			abort ();
		}
		
		void op_load_stack_addr (const Instruction & inst, std::ostream &) {
			
			assert (inst.size >= 0);
			size_t offset = 0;
			for (int i=0; i<inst.size; ++i) {
				offset += stackDataLayout [layoutFrame + i].getSize ();
			}
			
			*reinterpret_cast <size_t*> (&(stack [stackPos])) = size_t (&(stack [stackFrame])) + offset;
			stackPos += sizeof (size_t);
			stackDataLayout [dataLayoutPos++] = Layout (TYPE::usize);
			
		}
		
		void op_move (const Instruction & inst, std::ostream &) {
			abort ();
			stack [inst.size > 0 ? inst.size : inst.size + stackPos] = stack [stackPos];
			--stackPos;
		}
		
		void op_offset (const Instruction & inst, std::ostream & out) {
			auto layout = layouts [inst.symbol];
			size_t offset = layout.getOffset (inst.size);
			if (offset) {
				auto l = popTop ();
				auto lay = stackDataLayout [dataLayoutPos++];
				lay.literalToData (l + offset, &(stack [stackPos]));
				stackPos += lay.getSize ();
			}
		}
		
		void op_add (const Instruction & inst, std::ostream & out) {
			auto layoutRHS = stackDataLayout [dataLayoutPos - 1];
			auto layoutLHS = stackDataLayout [dataLayoutPos - 2];
			auto dest = &(stack [stackPos - layoutRHS.getSize () - layoutLHS.getSize ()]);
			layoutLHS.literalToData (
				layoutLHS.dataToLiteral (dest) + layoutRHS.dataToLiteral (dest + layoutLHS.getSize ()),
				dest
			);
			stackPos -= layoutRHS.getSize ();
			dataLayoutPos -= 1;
		}
		
		void op_and (const Instruction & inst, std::ostream & out) {
			abort ();
		}
		
		void op_cmp_eq (const Instruction & inst, std::ostream & out) {
			auto rhs = popTop ();
			auto lhs = popTop ();
			Layout boolLayout (TYPE::u8);
			boolLayout.literalToData (lhs == rhs, &(stack [stackPos]));
			stackDataLayout [dataLayoutPos++] = boolLayout;
			stackPos += boolLayout.getSize ();
		}
		
		void op_cmp_gt (const Instruction & inst, std::ostream & out) {
			abort ();
		}
		
		void op_cmp_gte (const Instruction & inst, std::ostream & out) {
			abort ();
		}
		
		void op_cmp_lt (const Instruction & inst, std::ostream & out) {
			abort ();
		}
		
		void op_cmp_lte (const Instruction & inst, std::ostream & out) {
			abort ();
		}
		
		void op_cmp_ne (const Instruction & inst, std::ostream & out) {
			abort ();
		}
		
		void op_div (const Instruction & inst, std::ostream & out) {
			auto layout = stackDataLayout [dataLayoutPos - 1];
			auto rhs = popTop ();
			auto lhs = popTop ();
			layout.literalToData (lhs / rhs, &(stack [stackPos]));
			stackDataLayout [dataLayoutPos++] = layout;
			stackPos += layout.getSize ();
		}
		
		void op_mul (const Instruction & inst, std::ostream & out) {
			auto layout = stackDataLayout [dataLayoutPos - 1];
			auto rhs = popTop ();
			auto lhs = popTop ();
			layout.literalToData (lhs * rhs, &(stack [stackPos]));
			stackDataLayout [dataLayoutPos++] = layout;
			stackPos += layout.getSize ();
		}
		
		void op_not (const Instruction & inst, std::ostream & out) {
			auto layout = stackDataLayout [dataLayoutPos - 1];
			auto literal = layout.dataToLiteral (&stack [stackPos - layout.getSize ()]);
			auto l = layout.literalToData (!literal, &stack [stackPos - layout.getSize ()]);
			assert (l == layout.getSize ());
		}
		
		void op_or (const Instruction & inst, std::ostream & out) {
			abort ();
		}
		
		void op_sub (const Instruction & inst, std::ostream & out) {
			auto layout = stackDataLayout [dataLayoutPos - 1];
			auto rhs = popTop ();
			auto lhs = popTop ();
			layout.literalToData (lhs - rhs, &(stack [stackPos]));
			stackDataLayout [dataLayoutPos++] = layout;
			stackPos += layout.getSize ();
		}
		
		void op_xor (const Instruction & inst, std::ostream & out) {
			abort ();
		}
		
		void op_pop (const Instruction & inst, std::ostream & out) {
			for (int i=0; i<inst.size; ++i) {
				stackPos -= stackDataLayout [--dataLayoutPos].getSize ();
			}
		}
		
		void op_reserve (const Instruction & inst, std::ostream & out) {
			auto itt = layouts.find (inst.symbol);
			assert (itt != layouts.end ());
			stackPos += itt->second.getSize ();
			stackDataLayout [dataLayoutPos++] = itt->second;
		}
		
		void op_ret (const Instruction & inst, std::ostream & out) {
			textRegister = textLength;
		}
		
		void op_size_of (const Instruction & inst, std::ostream & out) {
			abort ();
		}
		
		void op_store_inst_ptr (const Instruction & inst, std::ostream & out) {
			abort ();
		}
		
		virtual void run (const Chunk & chunk, std::ostream & out) {
			
			for (auto & pair : chunk.layouts) {
				layout (pair.first, pair.second, out);
			}
			
			for (auto & pair : chunk.functionLayouts) {
				functionLayouts [pair.first] = pair.second;
			}
			
			for (auto & t : chunk.data) {
				data (std::get <0> (t), std::get <1> (t), std::get <2> (t), out);
			}
			
			for (size_t i = 0, l = chunk.text.size (); i<l; ++i) {
				auto & inst = chunk.text [i];
				if (inst.opcode == OP_CODE::LABEL) {
					symbolTable [inst.symbol] = i;
				}
			}
			
			out << "================================\n";
			runFrom (chunk, out);
			
		}
		
	protected:
	private:
		
		void runFrom (const Chunk & chunk, std::ostream & out, size_t offset = 0) {
			
			PrintingEngine pe;
			for (textRegister = offset, textLength = chunk.text.size (); textRegister<textLength; ++textRegister) {
				
				out << layoutFrame << " " << dataLayoutPos << ", " << stackFrame << " " << stackPos << ": ";
				for (size_t i=layoutFrame, l=dataLayoutPos, offset=0; i<l; ++i) {
					auto layout = stackDataLayout [i];
					out << layout.dataToLiteral (&stack [stackFrame + offset]).toString (text::PrintMode::PLAIN) << " ";
					offset += layout.getSize ();
				}
				out << std::endl;
				
				auto & inst = chunk.text [textRegister];
				switch (inst.opcode) {
					case OP_CODE::CALL:
						pe.op_call (inst, out);
						op_call (inst, chunk, out);
						break;
					case OP_CODE::CALL_SYS:
						pe.op_call_sys (inst, out);
						op_call_sys (inst, out);
						break;
					case OP_CODE::COPY:
						pe.op_copy (inst, out);
						op_copy (inst, out);
						break;
					case OP_CODE::COPY_MEMCOPY:
						pe.op_copy_memcopy (inst, out);
						op_copy_memcopy (inst, out);
						break;
					case OP_CODE::DUPLICATE:
						pe.op_duplicate (inst, out);
						op_duplicate (inst, out);
						break;
					case OP_CODE::JMP:
						pe.op_jmp (inst, out);
						op_jmp (inst, out);
						break;
					case OP_CODE::JMP_IF:
						pe.op_jmp_if (inst, out);
						op_jmp_if (inst, out);
						break;
					case OP_CODE::JMP_VAR:
						pe.op_jmp_var (inst, out);
						op_jmp_var (inst, out);
						break;
					case OP_CODE::LABEL:
						pe.op_label (inst, out);
						op_label (inst, out);
						break;
					case OP_CODE::LOAD:
						pe.op_load (inst, out);
						op_load (inst, out);
						break;
					case OP_CODE::LOAD_GLOBAL_ADDR:
						pe.op_load_global_addr (inst, out);
						op_load_global_addr (inst, out);
						break;
					case OP_CODE::LOAD_INST_PTR:
						pe.op_load_inst_ptr (inst, out);
						op_load_inst_ptr (inst, out);
						break;
					case OP_CODE::LOAD_STACK_ADDR:
						pe.op_load_stack_addr (inst, out);
						op_load_stack_addr (inst, out);
						break;
					case OP_CODE::MOVE:
						pe.op_move (inst, out);
						op_move (inst, out);
						break;
					case OP_CODE::OFFSET:
						pe.op_offset (inst, out);
						op_offset (inst, out);
						break;
					case OP_CODE::OP_ADD:
						pe.op_add (inst, out);
						op_add (inst, out);
						break;
					case OP_CODE::OP_AND:
						pe.op_and (inst, out);
						op_and (inst, out);
						break;
					case OP_CODE::OP_CMP_EQ:
						pe.op_cmp_eq (inst, out);
						op_cmp_eq (inst, out);
						break;
					case OP_CODE::OP_CMP_GT:
						pe.op_cmp_gt (inst, out);
						op_cmp_gt (inst, out);
						break;
					case OP_CODE::OP_CMP_GTE:
						pe.op_cmp_gte (inst, out);
						op_cmp_gte (inst, out);
						break;
					case OP_CODE::OP_CMP_LT:
						pe.op_cmp_lt (inst, out);
						op_cmp_lt (inst, out);
						break;
					case OP_CODE::OP_CMP_LTE:
						pe.op_cmp_lte (inst, out);
						op_cmp_lte (inst, out);
						break;
					case OP_CODE::OP_CMP_NE:
						pe.op_cmp_ne (inst, out);
						op_cmp_ne (inst, out);
						break;
					case OP_CODE::OP_DIV:
						pe.op_div (inst, out);
						op_div (inst, out);
						break;
					case OP_CODE::OP_MUL:
						pe.op_mul (inst, out);
						op_mul (inst, out);
						break;
					case OP_CODE::OP_NOT:
						pe.op_not (inst, out);
						op_not (inst, out);
						break;
					case OP_CODE::OP_OR:
						pe.op_or (inst, out);
						op_or (inst, out);
						break;
					case OP_CODE::OP_SUB:
						pe.op_sub (inst, out);
						op_sub (inst, out);
						break;
					case OP_CODE::OP_XOR:
						pe.op_xor (inst, out);
						op_xor (inst, out);
						break;
					case OP_CODE::POP:
						pe.op_pop (inst, out);
						op_pop (inst, out);
						break;
					case OP_CODE::RESERVE:
						pe.op_reserve (inst, out);
						op_reserve (inst, out);
						break;
					case OP_CODE::RET:
						pe.op_ret (inst, out);
						op_ret (inst, out);
						break;
					case OP_CODE::SIZE_OF:
						pe.op_size_of (inst, out);
						op_size_of (inst, out);
						break;
					case OP_CODE::STORE_INST_PTR:
						pe.op_store_inst_ptr (inst, out);
						op_store_inst_ptr (inst, out);
						break;
				}
				
			}
			
		}
		
		BasicArray <Layout> stackDataLayout = BasicArray <Layout> (4096);
		BasicArray <uint8_t> stack = BasicArray <uint8_t> (4096*128);
		size_t stackFrame=0, stackPos=0, layoutFrame=0, dataLayoutPos=0, textRegister=0, textLength=0;
		std::map <symbol_t, Layout> layouts;
		std::map <symbol_t, std::pair <BasicArray <symbol_t>, BasicArray <symbol_t>>> functionLayouts;
		std::map <symbol_t, size_t> symbolTable;
		std::map <symbol_t, std::function <const BasicArray <Literal>(const BasicArray <Literal>, const AST::CallingData &)>> nativeFunctions;
		std::map <symbol_t, std::unique_ptr <uint8_t>> globals;
		int exitCode = 0;
		
};

typedef InterpretingStackMachine StackInterpreter;


}
