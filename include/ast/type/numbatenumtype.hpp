#ifndef NUMBATENUMTYPE_HPP
#define NUMBATENUMTYPE_HPP

#include "numbattype.hpp"


namespace numbat {
namespace parser {


class NumbatEnumType : public NumbatType {
	public:
		const shared_ptr <NumbatType> & getBaseType () const {return baseType;}
		virtual const ASTnode getArrayType () const {return baseType->getArrayType ();}
		virtual const bool isArray () const {return false;}
		virtual const bool isValid () const {return NumbatType::isValid () and baseType->isValid ();}
		virtual const size_t getBitSize () const {return baseType->getBitSize ();}
		//virtual string convertDataToString (const void * data) const {size_t offset=0; return convertDataToString (data, offset);}
		//virtual string convertDataToString (const void * data, size_t & offset) const;
		virtual string toString (const string & indent = "") const {return indent + "enum " + NumbatType::toString ();}
		
		NumbatEnumType () {}
		NumbatEnumType (const string & iden, const shared_ptr <NumbatType> & baseType, const std::set <string> & meta) : NumbatType (iden, meta), baseType (baseType) {}
	private:
		shared_ptr <NumbatType> baseType;
};


}
}

#endif /*NUMBATENUMTYPE_HPP*/