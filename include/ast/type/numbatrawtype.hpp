#ifndef NUMBATRAWTYPE_HPP
#define NUMBATRAWTYPE_HPP

#include "numbattype.hpp"


namespace numbat {
namespace parser {


class NumbatRawType : public NumbatType {
	public:
		enum Type {FLOAT, SIGNED, UNSIGNED};
		virtual const bool isFloat () const {return type == FLOAT;}
		virtual const bool isRaw () const {return true;}
		virtual const bool isSigned () const {return type == SIGNED;}
		virtual const bool isValid () const {return true;}
		virtual const size_t getBitSize () const {return size;}
		virtual const Type getRawType () const {return type;}
		virtual string convertDataToString (const void * data) const {size_t offset=0; return convertDataToString (data, offset);}
		virtual string convertDataToString (const void * data, size_t & offset) const;
		virtual string toString (const string & indent = "") const;
		
		NumbatRawType () {}
		NumbatRawType (string iden, size_t size, Type type, FunctionDecleration * mallocFunc, FunctionDecleration * freeFunc) : NumbatType (iden, mallocFunc, freeFunc), size (size), type (type) {} // for raw data
	private:
		size_t size=0;
		Type type;
};


}
}

#endif /*NUMBATRAWTYPE_HPP*/