#pragma once

#include <map>
#include <memory>
#include <nir/type.hpp>

namespace nir {


class PointerType : public Type {
	CONST_VISITABLE
	public:
		
		string toString (text::PrintMode mode=text::PLAIN) {if (this) return strDump (mode); else return "nullptr";}
		
		virtual ArithmaticType getArithmaticType () const {return Type::ArithmaticType::UINT;}
		virtual const Type * getDereferenceType () const {return type;}
		virtual const Type * getReferenceType () const {auto reft = type->getReferenceType (); return reft ? reft : this;}
		virtual const Type * getValueType () const {return type->getValueType ();}
		virtual size_t calculateSize (size_t ptrSize) const {return ptrSize;}
		
		static PointerType * pointerTo (const Type * t) {
			auto itt = pointerTypes.find (t);
			if (itt != pointerTypes.end ()) {
				return itt->second.get ();
			} else {
				return (pointerTypes [t] = std::unique_ptr <PointerType> (new PointerType (t))).get ();
			}
		}
		
	protected:
	private:
		
		PointerType (const Type * type) : type (type) {}
		
		virtual string strDump (text::PrintMode mode) const {return type->toString (mode) + "*";}
		
		
		const Type * type;
		
		static std::map <const Type *, std::unique_ptr <PointerType>> pointerTypes;
		
};


};
