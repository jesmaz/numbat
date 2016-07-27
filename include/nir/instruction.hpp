#ifndef NIR_INSTRUCTION
#define NIR_INSTRUCTION


#include <nir/forward.hpp>
#include <nir/type.hpp>
#include <visitor.hpp>

#include <cassert>
#include <vector>


namespace nir {

using std::string;

typedef const string * symbol;

struct Argument {
	const Instruction * instr;
	symbol sym;
	string toString (text::PrintMode mode=text::PLAIN) const;
};

std::vector <const Type *> argumentToType (const std::vector <Argument> & args);
std::vector <symbol> argumentToSymbol (const std::vector <Argument> & args);

class Instruction : public numbat::visitor::BaseConstVisitable {
	
	public:
		
		virtual bool validate () const=0;
		
		const Type * getType () const {return types [0];}
		
		string printIden (text::PrintMode mode=text::PLAIN) const;
		string toString (text::PrintMode mode=text::PLAIN) const;
		symbol getIden () const {return idens [0];}
		
		const std::vector <Argument> & getArguments () const {return arguments;}
		const std::vector <const Type *> & getTypes () const {return types;}
		const std::vector <symbol> & getIdens () const {return idens;}
		
		virtual const nir::Type * resolveType (const string & iden) const {return nullptr;}
		virtual const nir::Instruction * resolve (nir::Scope * scope, const string & iden) const {return nullptr;}
		
	protected:
		
		Instruction (const std::vector <Argument> & arguments, const std::vector <const Type *> & types, const std::vector <symbol> & idens) : arguments (arguments), types (types), idens (idens) {}
		
	private:
		
		virtual string strDump (text::PrintMode mode) const=0;
		
		std::vector <Argument> arguments;
		std::vector <const Type *> types;
		std::vector <symbol> idens;
		
};

};


#endif/*NIR_TYPE_BASE_HPP*/