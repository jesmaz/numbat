#ifndef NIR_INSTRUCTION
#define NIR_INSTRUCTION


#include "../visitor.hpp"
#include "type.hpp"

#include <cassert>


namespace nir {

using std::string;

typedef const string * symbol;

class Instruction : public numbat::visitor::BaseConstVisitable {
	
	public:
		
		virtual bool validate () const=0;
		string printIden (text::PrintMode mode=text::PLAIN) const;
		string toString (text::PrintMode mode=text::PLAIN) const;
		
		const Type * const getType () const {return type;}
		symbol getIden () const {return iden;}
		
		virtual const nir::Type * resolveType (const string & iden) const {return nullptr;}
		virtual const nir::Instruction * resolve (nir::Scope * scope, const string & iden) const {return nullptr;}
		
	protected:
		
		Instruction (const Type * type, symbol iden) : type (type), iden (iden) {}
		
	private:
		
		virtual string strDump (text::PrintMode mode) const=0;
		
		const Type * type;
		symbol iden;
		
};

};


#endif/*NIR_TYPE_BASE_HPP*/