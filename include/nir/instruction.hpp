#ifndef NIR_INSTRUCTION
#define NIR_INSTRUCTION


#include "../visitor.hpp"
#include "type.hpp"

#include <cassert>
#include <vector>


namespace nir {

using std::string;

typedef const string * symbol;

class Instruction : public numbat::visitor::BaseConstVisitable {
	
	public:
		
		virtual bool validate () const=0;
		
		const Type * getType () const {return types [0];}
		
		string printIden (text::PrintMode mode=text::PLAIN) const;
		string toString (text::PrintMode mode=text::PLAIN) const;
		symbol getIden () const {return idens [0];}
		
		std::vector <const Type *> getTypes () const {return types;}
		std::vector <symbol> getIdens () const {return idens;}
		
		virtual const nir::Type * resolveType (const string & iden) const {return nullptr;}
		virtual const nir::Instruction * resolve (nir::Scope * scope, const string & iden) const {return nullptr;}
		
	protected:
		
		Instruction (std::vector <const Type *> types, std::vector <symbol> idens) : types (types), idens (idens) {}
		
	private:
		
		virtual string strDump (text::PrintMode mode) const=0;
		
		std::vector <const Type *> types;
		std::vector <symbol> idens;
		
};

};


#endif/*NIR_TYPE_BASE_HPP*/