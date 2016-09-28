#include <cassert>
#include <nir/value.hpp>


namespace nir {


template <typename T>
class GenericValue : public AbstractValue {
	CONST_VISITABLE
	public:
		
		virtual string toString (text::PrintMode mode) const {return std::to_string (val);}
		
		virtual Value dereference () const {abort ();}
		virtual void emplace (const Value &) {abort ();}
		
		virtual operator double () const {return double (val);}
		virtual operator int64_t () const {return int64_t (val);}
		virtual operator uint64_t () const {return uint64_t (val);}
		
		GenericValue (const T & v) : val (v) {}
		
	protected:
	private:
		
		T val;
		
};

class TupleValue : public AbstractValue {
	CONST_VISITABLE
	public:
		
		virtual string toString (text::PrintMode mode) const {
			string s = "(";
			for (const Value & v : members) {
				s += v->toString (mode) + ", ";
			}
			s.pop_back ();
			s.pop_back ();
			s += ")";
			return s;
		}
		
		virtual Value dereference () const {abort ();}
		virtual void emplace (const Value &) {abort ();}
		
		virtual operator double () const {abort ();}
		virtual operator int64_t () const {abort ();}
		virtual operator uint64_t () const {abort ();}
		
		std::vector <Value> & getMembers () {return members;}
		const std::vector <Value> & getMembers () const {return members;}
		
		TupleValue (const std::vector <Value> & members) : members (members) {}
		
	protected:
	private:
		
		std::vector <Value> members;
		
};

class PointerValue : public AbstractValue {
	CONST_VISITABLE
	public:
		
		virtual string toString (text::PrintMode mode) const {
			return "*" + initialVal->toString (mode);
		}
		
		virtual Value dereference () const {
			if (index >= 0) {
				const TupleValue * tuple = reinterpret_cast <const TupleValue *> (initialVal.get ());
				assert (typeid (*tuple) == typeid (TupleValue));
				auto & members = tuple->getMembers ();
				assert (members.size () < index);
				return members [index];
			} else {
				return initialVal;
			}
		}
		
		virtual void emplace (const Value & val) {
			if (index >= 0) {
				TupleValue * tuple = reinterpret_cast <TupleValue *> (initialVal.get ());
				assert (typeid (*tuple) == typeid (TupleValue));
				auto & members = tuple->getMembers ();
				assert (members.size () < index);
				members [index] = val;
			} else {
				initialVal = val;
			}
		}
		
		virtual operator double () const {abort ();}
		virtual operator int64_t () const {abort ();}
		virtual operator uint64_t () const {abort ();}
		
		PointerValue (const Value & initialVal, ssize_t index) : initialVal (initialVal), index (index) {}
		
	protected:
	private:
		
		Value initialVal;
		ssize_t index;
		
};


Value::Value () {}
Value::Value (AbstractValue * absVal) : val (absVal) {}
Value::Value (const Function * func) {abort ();}
Value::Value (const std::vector <Value> & members) : val (new TupleValue (members)) {}

Value::Value (double d) : val (new GenericValue <double> (d)) {}
Value::Value (int8_t d) : val (new GenericValue <int8_t> (d)) {}
Value::Value (int16_t d) : val (new GenericValue <int16_t> (d)) {}
Value::Value (int32_t d) : val (new GenericValue <int32_t> (d)) {}
Value::Value (int64_t d) : val (new GenericValue <int64_t> (d)) {}
Value::Value (uint8_t d) : val (new GenericValue <uint8_t> (d)) {}
Value::Value (uint16_t d) : val (new GenericValue <uint16_t> (d)) {}
Value::Value (uint32_t d) : val (new GenericValue <uint32_t> (d)) {}
Value::Value (uint64_t d) : val (new GenericValue <uint64_t> (d)) {}

Value Value::referenceTo (ssize_t index) {return Value (new PointerValue (*this, index));}

}
