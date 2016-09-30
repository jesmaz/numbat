#include <nir/block.hpp>
#include <nir/instruction.hpp>


namespace nir {


const Block::Iterator & Block::Iterator::operator++ () {
	++pos;
	if (not *this) {
		if (block->fallthrough) {
			block = block->fallthrough;
			pos = 0;
		}
	}
	return *this;
}

const Instruction & Block::Iterator::operator * () {
	return *block->instructions [pos];
}

Block::Iterator::operator bool () const {
	return pos < block->instructions.size ();
}


std::ostream & operator << (std::ostream & out, const Block::PrintIterator & itt) {
	if (itt.printedName) {
		out << itt.block->getInstructions () [itt.pos]->toString (itt.pm);
	} else {
		out << *itt.block->getName ();
	}
	return out;
}


const Block::PrintIterator & Block::PrintIterator::operator++ () {
	if (printedName) {
		++pos;
		if (not *this) {
			if (block->fallthrough) {
				block = block->fallthrough;
				pos = 0;
				printedName = false;
			}
		}
	} else {
		printedName = true;
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
