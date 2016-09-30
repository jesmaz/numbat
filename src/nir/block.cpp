#include <nir/block.hpp>
#include <nir/instruction.hpp>


namespace nir {


std::ostream & operator << (std::ostream & out, const Block::PrintIterator & itt) {
	out << itt.block->getInstructions () [itt.pos]->toString (text::PLAIN);
	return out;
}


const Block::PrintIterator & Block::PrintIterator::operator++ () {
	++pos;
	if (not *this) {
		if (block->fallthrough) {
			block = block->fallthrough;
			pos = 0;
		}
	}
	return *this;
}

Block::PrintIterator::operator bool () const {
	return pos < block->instructions.size ();
}

bool Block::validate () const {
	
	bool valid = true;
	for (const Instruction * instr : instructions) {
		valid &= instr->validate ();
	}
	return valid;
	
}

const Instruction * Block::give (const Instruction * instr) {
	instructions.push_back (instr);
	return instr;
}

}
