#include <nir/instruction.hpp>


namespace nir {


Argument::Argument (const Instruction * instr, size_t index) {
	auto res = instr->getResults () [index];
	sym = res.iden;
	type = res.type;
}

Argument::Argument (const Instruction * instr, symbol s) : sym (s), type (nullptr) {
	for (auto & res : instr->getResults ()) {
		if (res.iden == s) {
			type = res.type;
		}
	}
	assert (type);
}


BasicArray <const Type *> argumentToType (const BasicArray <Argument> & args) {
	return args.map <const Type *> ([](const Argument & a){return a.type;});
}

BasicArray <symbol> argumentToSymbol (const BasicArray <Argument> & args) {
	return args.map <symbol> ([](const Argument & arg){
		return arg.sym;
	});
}


string Argument::toString (text::PrintMode mode) const {
	
	return *sym;
	
}


string Instruction::printIden (text::PrintMode mode) const {
	if (this) {
		string s;
		for (auto res : results) {
			if (res.iden) {
				s += *res.iden;
			} else {
				s += strDump (mode);
			}
			s += " ";
		}
		s.pop_back ();
		return s;
	} else {
		return "nullptr";
	}
}

string Instruction::toString (text::PrintMode mode) const {
	if (this) {
		string s;
		for (auto & r : getResults ()) {
			if (r.iden) {
				s += *r.iden;
			}
		}
		return s + ": " + strDump (mode);;
	} else {
		return "nullptr";
	}
}


};
