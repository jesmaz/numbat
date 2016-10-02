#include "../../include/nir/block.hpp"
#include "../../include/nir/function.hpp"
#include "../../include/nir/parameter.hpp"
#include "../../include/nir/type/functionType.hpp"


namespace nir {


auto getInstType = [](const BasicArray <const Parameter *> & args) {
	return args.map <const Type *> ([](const Parameter * p){return p->getType ();});
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

void Function::pushBlock (Block * block) {
	blocks.back ()->setFallthrough (block);
	blocks.push_back (block);
}

Function::Function (const std::string & label, LINKAGE linkage) : entry (new Block (&(this->label))), blocks ({entry}), label (label), linkage (linkage) {}

Function::Function (const BasicArray <const Parameter *> & args, const BasicArray <const Parameter *> & ret, const std::string & label, LINKAGE linkage) : entry (new Block (&(this->label))), blocks ({entry}), label (label), args (args), ret (ret), retTypes (getInstType (ret)), type (FunctionType::get (getInstType (args), retTypes)), linkage (linkage) {}


};
