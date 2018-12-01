#pragma once


#include <forward.hpp>
#include <utility/literals.hpp>
#include <utility/symbol.hpp>


namespace stackmachine {


enum class OP_CODE {
	// [args ...][func*] (function sig) -> [rets ...]
	CALL,
	// [args ...] (system call name) -> [ret]
	CALL_SYS,
	// [arg] (destination layout) -> [arg]
	CONVERT,
	// [var*][data] (layout) -> []
	COPY,
	// [dest*][src*][size] -> []
	COPY_MEMCOPY,
	
	// [arg][...] (stack_offset) -> [arg][...][arg]
	DUPLICATE,
	
	// (symbol) -> []
	JMP,
	// [cond] (symbol) -> []
	JMP_IF,
	// [dest] -> []
	JMP_VAR,
	
	// (symbol) -> []
	LABEL,
	
	// [var*] (layout) -> [data]
	LOAD,
	// (addr) -> [data]
	LOAD_GLOBAL_ADDR,
	// -> [inst_ptr]
	LOAD_INST_PTR,
	// (offset) -> [var*]
	LOAD_STACK_ADDR,
	
	// [dest][...][top] (stack_offset) -> [top][...]
	MOVE,
	
	// [var*] (index, layout) -> [member*]
	OFFSET,
	
	// [lhs][rhs] -> [result]
	OP_ADD,
	OP_AND,
	OP_CMP_EQ,
	OP_CMP_GT,
	OP_CMP_GTE,
	OP_CMP_LT,
	OP_CMP_LTE,
	OP_CMP_NE,
	OP_DIV,
	OP_MUL,
	
	// [arg] -> result
	OP_NOT,
	
	// [lhs][rhs] -> [result]
	OP_OR,
	OP_SUB,
	OP_XOR,
	
	// [...] (amount) -> []
	POP,
	// (layout) -> [uninitalised data]
	RESERVE,
	// [...] -> [...]
	RET,
	// (layout) -> [size]
	SIZE_OF,
	// [inst*] -> []
	STORE_INST_PTR
};

enum class TYPE : uint16_t {
	f16, f32, f64,
	function,
	i8, i16, i32, i64, isize,
	u8, u16, u32, u64, usize,
	
	_META_ARRAY_OF,
	_META_FUNC_SPLIT,
	_META_STRUCT_BEGIN,
	_META_STRUCT_END
};


struct Instruction {
	
	Instruction () : symbol () {}
	Instruction (OP_CODE opcode) : opcode (opcode), size (0) {}
	Instruction (OP_CODE opcode, int size) : opcode (opcode), size (size) {}
	Instruction (OP_CODE opcode, symbol_t symbol) : opcode (opcode), symbol (symbol) {}
	Instruction (OP_CODE opcode, int size, symbol_t symbol) : opcode (opcode), size (size), symbol (symbol) {}
	
	OP_CODE opcode;
	union {
		struct {
			TYPE destType, sourceType;
		};
		int size;
	};
	symbol_t symbol;
	
};

struct Layout {
	
	public:
		
		Literal dataToLiteral (const uint8_t * data) const;
		size_t literalToData (const Literal & literal, uint8_t * data, bool init=false) const;
		
		uint32_t getOffset (size_t pos) const {return offsets [pos];}
		uint32_t getAlignment () const {return alignment;}
		uint32_t getSize () const {return size;}
		
		Layout () : size (0), alignment (1) {}
		Layout (const TYPE type);
		Layout (const BasicArray <TYPE> & members) : Layout (members.begin (), members.end ()) {}
		Layout (BasicArray <TYPE>::const_iterator beg, const BasicArray <TYPE>::const_iterator end) : Layout (&beg, end) {}
		
	protected:
	private:
		
		Layout (BasicArray <TYPE>::const_iterator * beg, const BasicArray <TYPE>::const_iterator end);
		
		uint32_t size, alignment;
		TYPE type;
		BasicArray <Layout> components;
		BasicArray <uint32_t> offsets;
		
};


}
