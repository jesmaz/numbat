#pragma once

#include <cinttypes>
#include <memory>
#include <nir/forward.hpp>
#include <string>
#include <utility/text.hpp>
 
#include <visitor.hpp>


namespace nir {


using std::string;

struct Value;

class AbstractValue : public numbat::visitor::BaseConstVisitable {
	
	public:
		
		virtual const AbstractValue & operator = (const AbstractValue & rhs)=0;
		
		virtual string toString (text::PrintMode mode=text::PrintMode::PLAIN) const=0;
		
		virtual const Type * getType () const=0;
		
		virtual Value dereference () const=0;
		virtual Value operator [] (const Parameter * param) const=0;
		virtual Value operator [] (size_t index) const=0;
		virtual Value reinterpret (const Type * type) const=0;
		
		virtual operator double () const=0;
		virtual operator int64_t () const=0;
		virtual operator uint64_t () const=0;
		
	protected:
	private:
		
};

struct Value {
	
	public:
		
		Value ();
		Value (AbstractValue * absVal);
		Value (const Function * func);
		Value (const DynArray <Value> & members, const Struct * layout);
		Value (const Value & val) : val (val.val) {}
		Value (double);
		Value (float);
		Value (int8_t);
		Value (int16_t);
		Value (int32_t);
		Value (int64_t);
		Value (uint8_t);
		Value (uint16_t);
		Value (uint32_t);
		Value (uint64_t);
		
		AbstractValue & operator * () {return *val;}
		const AbstractValue & operator * () const {return *val;}
		
		AbstractValue * operator -> () {return val.get ();}
		const AbstractValue * operator -> () const {return val.get ();}
		
		AbstractValue * get () {return val.get ();}
		const AbstractValue * get () const {return val.get ();}
		
		Value referenceTo ();
		
		static Value allocate (const Type * type, int64_t amount);
		
		operator bool () const {return bool (val);}
		
	private:
		
		std::shared_ptr <AbstractValue> val;
		
};

}
