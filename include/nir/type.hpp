#pragma once


#include <nir/forward.hpp>
#include <string>
#include <utility/text.hpp>
#include <visitor.hpp>


namespace nir {

using std::string;

class Type : public numbat::visitor::BaseConstVisitable {
	
	public:
		
		enum ArithmaticType : char {DEFAULT=0, DECINT='d', INT='i', FPINT='f', UINT='u'};
		
		virtual bool validate () const {return true;}
		
		string toString (text::PrintMode mode=text::PLAIN) const {if (this) return strDump (mode); else return "nullptr";}
		
		virtual const Type * getDereferenceType () const {return nullptr;}
		const Type * getPointerTo () const;
		
		virtual size_t calculateSize (size_t ptrSize) const=0;//Expects bytes
		virtual ssize_t calculateOffset (size_t ptrSize, const string & iden) const {return -1;}//Expects bytes
		
		virtual ArithmaticType getArithmaticType () const {return ArithmaticType::DEFAULT;}
		
	protected:
	private:
		
		virtual string strDump (text::PrintMode mode) const=0;
		
};

};
