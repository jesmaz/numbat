#pragma once


#include <algorithm>
#include <stackmachine/instruction.hpp>
#include <utility/array.hpp>
#include <utility/literals.hpp>
#include <utility/symbol.hpp>


namespace stackmachine {


struct Chunk {
	BasicArray <std::pair <symbol_t, BasicArray <TYPE>>> layouts;
	BasicArray <std::pair <symbol_t, std::pair <BasicArray <symbol_t>, BasicArray <symbol_t>>>> functionLayouts;
	BasicArray <std::tuple <symbol_t, symbol_t, Literal>> data;
	BasicArray <Instruction> text;
};

inline const Chunk operator + (const Chunk & lhs, const Chunk & rhs) {
	Chunk c {
		lhs.layouts + rhs.layouts,
		lhs.functionLayouts + rhs.functionLayouts,
		lhs.data + rhs.data,
		lhs.text + rhs.text
	};
	std::inplace_merge (c.layouts.begin (), c.layouts.begin () + lhs.layouts.size (), c.layouts.end ());
	std::inplace_merge (c.functionLayouts.begin (), c.functionLayouts.begin () + lhs.functionLayouts.size (), c.functionLayouts.end ());
	std::inplace_merge (c.data.begin (), c.data.begin () + lhs.data.size (), c.data.end ());
	return c;
}

inline const Chunk & operator += (Chunk & lhs, const Chunk & rhs) {
	lhs.layouts = lhs.layouts + rhs.layouts;
	lhs.functionLayouts = lhs.functionLayouts + rhs.functionLayouts;
	lhs.data = lhs.data + rhs.data;
	lhs.text = lhs.text + rhs.text;
	std::inplace_merge (lhs.layouts.begin (), lhs.layouts.end () - rhs.layouts.size (), lhs.layouts.end ());
	std::inplace_merge (lhs.functionLayouts.begin (), lhs.functionLayouts.end () - rhs.functionLayouts.size (), lhs.functionLayouts.end ());
	std::inplace_merge (lhs.data.begin (), lhs.data.end () - rhs.data.size (), lhs.data.end ());
	return lhs;
}


}
