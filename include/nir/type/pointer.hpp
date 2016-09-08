#pragma once

#include <map>
#include <nir/type.hpp>

namespace nir {


class PointerType : public Type {
	CONST_VISITABLE
	public:
		
		string toString (text::PrintMode mode=text::PLAIN) {if (this) return strDump (mode); else return "nullptr";}
		
		virtual ArithmaticType getArithmaticType () const {return Type::ArithmaticType::UINT;}
		virtual const Type * getDereferenceType () const {return type;}
		virtual size_t calculateSize (size_t ptrSize) const {return ptrSize;}
		
		static PointerType * pointerTo (const Type * t) {
			auto itt = pointerTypes.find (t);
			if (itt != pointerTypes.end ()) {
				return itt->second;
			} else {
				return pointerTypes [t] = new PointerType (t);
			}
		}
		
	protected:
	private:
		
		PointerType (const Type * type) : type (type) {}
		
		virtual string strDump (text::PrintMode mode) const {return type->toString (mode) + "*";}
		
		
		const Type * type;
		
		static std::map <const Type *, PointerType *> pointerTypes;
		
};


};
