#ifndef NUMBATPOINTERTYPE_HPP
#define NUMBATPOINTERTYPE_HPP

#include "numbattype.hpp"


namespace numbat {
namespace parser {


class NumbatPointerType : public NumbatType {
	public:
		const ASTnode & getDataType () const {return dataType;}
		virtual const ASTnode getArrayType () const {return dataType;}
		virtual const bool isArray () const {return true;}
		virtual const bool isValid () const;
		virtual const size_t getBitSize () const {return 64;}
		virtual const ssize_t getLengthIndex () const {return 0;}
		virtual string convertDataToString (const void * data) const {size_t offset=0; return convertDataToString (data, offset);}
		virtual string convertDataToString (const void * data, size_t & offset) const;
		virtual string toString (const string & indent = "") const;
		
		NumbatPointerType () {}
		NumbatPointerType (string iden, ASTnode dataType) : NumbatType (iden), dataType (dataType) {}
	private:
		ASTnode dataType;
};


}
}

#endif /*NUMBATPOINTERTYPE_HPP*/