#pragma once

#include <cinttypes>
#include <memory>
#include <nir/forward.hpp>
#include <string>
#include <utility/text.hpp>
#include <vector>
#include <visitor.hpp>


namespace nir {


using std::string;

struct Value;

class AbstractValue : public numbat::visitor::BaseConstVisitable {
	
	public:
		
		virtual string toString (text::PrintMode mode) const=0;
		
		virtual Value dereference () const=0;
		virtual void emplace (const Value & val)=0;
		
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
		Value (const std::vector <Value> & members);
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
		
		Value referenceTo (ssize_t index=-1);
		
		static Value defCtr (const Type * type);
		
	private:
		
		std::shared_ptr <AbstractValue> val;
		
};

}
