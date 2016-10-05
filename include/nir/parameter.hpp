#pragma once

#include <nir/forward.hpp>
#include <nir/type.hpp>
#include <parse/tree/base.hpp>


namespace nir {


struct Parameter {
	public:
		
		bool validate () const {return true;}
		
		parser::PTNode getDefVal () const {return defVal;}
		const string & getIden () const {return iden;}
		const Type * getType () const {return type;}
		
		string toString (text::PrintMode mode) const {
			string s = type->toString (mode) + " " + iden;
			if (defVal) {
				s += ": " + defVal->toString (mode);
			}
			return s;
		}
		
		Parameter (parser::PTNode instruction, const Type * type, const string & iden) : defVal (instruction), type (type), iden (iden) {}
		Parameter (const Type * type, const string & iden) : defVal (nullptr), type (type), iden (iden) {}
		
	protected:
	private:
		
		parser::PTNode defVal;
		const Type * type;
		string iden;
		
};


};
