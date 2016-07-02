#include "../../include/nir/block.hpp"
#include "../../include/nir/function.hpp"
#include "../../include/nir/parameter.hpp"
#include "../../include/nir/type/functionType.hpp"


namespace nir {


auto getInstType = [](const std::vector <const Parameter *> & args) {
	std::vector <const Type *> type;
	type.reserve (args.size ());
	for (auto * arg : args) {
		type.push_back (arg->getType ());
	}
	return type;
};


bool Function::validate () const {
	
	bool valid = true;
	for (const Parameter * param : args) {
		valid &= param->validate ();
	}
	
	for (const Parameter * r : ret) {
		valid &= r->validate ();
	}
	
	for (const Block * block : blocks) {
		valid &= block->validate ();
	}
	return valid;
	
}


Function::Function () : entry (new Block), blocks ({entry}) {}

Function::Function (std::vector <const Parameter *> args, std::vector <const Parameter *> ret) : entry (new Block), blocks ({entry}), args (args), ret (ret), retTypes (getInstType (ret)), type (FunctionType::get (getInstType (args), retTypes)) {}


};