#include <algorithm>
#include <cassert>
#include <nir/parameter.hpp>
#include <nir/type/array.hpp>
#include <nir/type/number.hpp>
#include <nir/type/pointer.hpp>
#include <nir/type/struct.hpp>
#include <nir/value.hpp>


namespace nir {


struct ptr_details {
	size_t owners;
	void * start, * end;
	void (*del)(void*);
	ptr_details * child;
	
	~ptr_details () {
		del (start);
		if (child and not --child->owners) {
			delete child;
		}
	}
};

template <typename T>
struct adv_ptr {
	
	public:
		
		adv_ptr () : ptr (nullptr), details (nullptr) {}
		adv_ptr (size_t amount) : ptr (new T [amount]), details (new ptr_details) {
			details->owners = 1;
			details->start = ptr;
			details->end = ptr + amount;
			details->del = [](void * ptr) {delete [] reinterpret_cast <T*> (ptr);};
			details->child = nullptr;
		}
		adv_ptr (size_t amount, const T & val) : adv_ptr (amount) {
			std::fill (ptr, ptr + amount, val);
		}
		adv_ptr (T * ptr) : ptr (ptr), details (nullptr) {
			if (ptr) {
				details = new ptr_details;
				details->owners = 1;
				details->start = ptr;
				details->end = ptr + 1;
				details->del = [](void * ptr) {delete reinterpret_cast <T*> (ptr);};
				details->child = nullptr;
			}
		}
		adv_ptr (const adv_ptr <T> & other) : ptr (other.ptr), details (other.details) {
			if (details) details->owners += 1;
		}
		adv_ptr (const adv_ptr <T> && other) : ptr (other.ptr), details (other.details) {}
		
		~adv_ptr () {
			if (details and not --details->owners) {delete details;}
		}
		
		template <typename Y>
		adv_ptr <Y> reinterpret () const {return adv_ptr <Y> (reinterpret_cast <Y*> (ptr), details);}
		
		adv_ptr <T> operator + (size_t offset) const {return adv_ptr <T> (ptr + offset, details);}
		adv_ptr <T> operator - (size_t offset) const {return *this + -offset;}
		
		const adv_ptr <T> & operator = (const adv_ptr <T> & other) {
			if (details and not --details->owners) {delete details;}
			ptr = other.ptr;
			details = other.details;
			if (details) details->owners += 1;
			return *this;
		}
		
		T & operator * () {return *ptr;}
		const T & operator * () const {return *ptr;}
		
		T * operator -> () {return ptr;}
		const T * operator -> () const {return ptr;}
		
		T * get () {return ptr;}
		const T * get () const {return ptr;}
		
		T * itt () {return ptr;}
		const T * itt () const {return ptr;}
		
		T * end () {return reinterpret_cast <T*> (details->end);}
		const T * end () const {return reinterpret_cast <const T*> (details->end);}
		
	protected:
	private:
		
		adv_ptr (T * ptr, ptr_details * details) : ptr (ptr), details (details) {if (details) details->owners += 1;}
		
		template <typename Y>
		friend struct adv_ptr;
		
		template <typename Y>
		friend adv_ptr <Y*> refToo (const adv_ptr <Y> & other);
		
		template <typename Y>
		friend adv_ptr <Y> deRef (const adv_ptr <Y*> & other);
		
		T * ptr;
		ptr_details * details;
		
};

template <typename T>
adv_ptr <T*> refToo (const adv_ptr <T> & other) {
	adv_ptr <T*> ret (new T* (other.ptr));
	ret.details->child = other.details;
	other.details->owners += 1;
	return ret;
}

template <typename T>
adv_ptr <T> deRef (const adv_ptr <T*> & other) {
	adv_ptr <T> ret (*(other.ptr), other.details->child);
	return ret;
}

Value fromData (const adv_ptr <uint8_t> & ptr, const Type * type);


template <typename T>
class GenericValue : public AbstractValue {
	CONST_VISITABLE
	public:
		
		virtual const AbstractValue & operator = (const AbstractValue & rhs) {
			if (typeid (*this) == typeid (rhs)) {
				*val = *reinterpret_cast <const GenericValue <T> &> (rhs).val;
			} else {
				const Number * num = reinterpret_cast <const Number *> (rhs.getType ());
				assert (typeid (*num) == typeid (Number));
				switch (num->getArithmaticType ()) {
					case Type::DECINT:
					case Type::DEFAULT:
						abort ();
					case Type::FPINT:
						*val = double (rhs);
						break;
					case Type::INT:
						*val = int64_t (rhs);
						break;
					case Type::UINT:
						*val = uint64_t (rhs);
						break;
				}
			}
			return *this;
		}
		
		virtual string toString (text::PrintMode mode) const {
			auto itt = val.itt ();
			auto end = val.end ();
			if (end == itt+1) {
				return std::to_string (*val);
			}
			string s = "[";
			while (itt != end) {
				s += std::to_string (*itt) + ", ";
				++itt;
			}
			s.pop_back ();
			s.back () = ']';
			return s;
		}
		
		virtual const Type * getType () const {return numType;}
		
		virtual Value dereference () const {abort ();}
		virtual Value operator [] (const Parameter *) const {abort ();}
		virtual Value operator [] (size_t index) const {
			return Value (new GenericValue <T> (val + index, numType));
		}
		
		virtual operator double () const {return double (*val);}
		virtual operator int64_t () const {return int64_t (*val);}
		virtual operator uint64_t () const {return uint64_t (*val);}
		
		GenericValue (const T value, const Number * numType) : val (new T (value)), numType (numType) {}
		GenericValue (size_t amount, const T value, const Number * numType) : val (amount, value), numType (numType) {}
		GenericValue (const adv_ptr <T> location, const Number * numType) : val (location), numType (numType) {}
		
	protected:
	private:
		
		adv_ptr <T> val;
		const Number * numType;
		
};

class TupleValue : public AbstractValue {
	CONST_VISITABLE
	public:
		
		virtual const AbstractValue & operator = (const AbstractValue & rhs) {
			assert (typeid (*this) == typeid (rhs));
			const TupleValue & other = reinterpret_cast <const TupleValue &> (rhs);
			assert (layout == other.layout);
			auto itt_src = other.bytes;
			auto itt_dest = bytes;
			for (const Parameter * param : layout->getMemberArr ()) {
				const Type * type = param->getType ();
				*fromData (itt_dest, type) = *fromData (itt_src, type);
				ssize_t size = type->calculateSize (sizeof (Value));
				assert (size >= 0);
				itt_src = itt_src + size;
				itt_dest = itt_dest + size;
			}
			std::copy (other.bytes.get (), other.bytes.get () + layout->calculateSize (sizeof (Value)), bytes.get ());
			return *this;
		}
		
		virtual string toString (text::PrintMode mode) const {
			string s = "(";
			auto itt = bytes;
			for (const Parameter * param : layout->getMemberArr ()) {
				const Type * type = param->getType ();
				s += fromData (itt, type)->toString (mode) + ", ";
				itt = itt + type->calculateSize (sizeof (Value));
			}
			s.pop_back ();
			s.pop_back ();
			s += ")";
			return s;
		}
		
		virtual const Type * getType () const {return layout;}
		
		virtual Value dereference () const {abort ();}
		virtual Value operator [] (const Parameter * param) const;
		virtual Value operator [] (size_t index) const {
			adv_ptr <uint8_t> ptr = bytes + index * layout->calculateSize (sizeof (Value));
			return Value (new TupleValue (ptr, layout));
		}
		
		virtual operator double () const {abort ();}
		virtual operator int64_t () const {abort ();}
		virtual operator uint64_t () const {abort ();}
		
		TupleValue (const Struct * layout) : bytes (layout->calculateSize (sizeof (Value)), 0), layout (layout) {}
		TupleValue (size_t amount, const Struct * layout) : bytes (amount * layout->calculateSize (sizeof (Value)), 0), layout (layout) {}
		TupleValue (const adv_ptr <uint8_t> & bytes, const Struct * layout) : bytes (bytes), layout (layout) {}
		
	protected:
	private:
		
		adv_ptr <uint8_t> bytes;
		const Struct * layout;
		
};

class PointerValue : public AbstractValue {
	CONST_VISITABLE
	public:
		
		virtual const AbstractValue & operator = (const AbstractValue & rhs) {
			assert (typeid (*this) == typeid (rhs));
			const PointerValue & other = reinterpret_cast <const PointerValue &> (rhs);
			*ptr = *other.ptr;
			return *this;
		}
		
		virtual string toString (text::PrintMode mode) const {
			if (*ptr) {
				return "*" + dereference ()->toString (mode);
			} else {
				return "*null";
			}
		}
		
		virtual const Type * getType () const {return ptrType;}
		
		virtual Value dereference () const {
			const Type * type = ptrType->getDereferenceType ();
			const Value & value = *ptr;
			assert (value->getType () == type);
			return value;
		}
		
		virtual Value operator [] (const Parameter *) const {abort ();}
		virtual Value operator [] (size_t) const {abort ();}
		
		virtual operator double () const {abort ();}
		virtual operator int64_t () const {abort ();}
		virtual operator uint64_t () const {abort ();}
		
		PointerValue (const adv_ptr <Value> & ptr, const PointerType * ptrType) : ptr (ptr), ptrType (ptrType) {
			const uint64_t * dat = reinterpret_cast <const uint64_t *> (ptr.get ());
			if (dat [0] == 0 and dat [1] == 0) {
				// Assuming that the space is uninitialised
				new (reinterpret_cast <void *> (this->ptr.get ())) Value ();
			}
		}
		
	protected:
	private:
		
		adv_ptr <Value> ptr;
		const PointerType * ptrType;
		
};


Value TupleValue::operator [] (const Parameter * param) const {
	ssize_t offset = layout->calculateOffset (sizeof (Value), *param->getIden ());
	assert (offset >= 0);
	adv_ptr <uint8_t> ptr = bytes + offset;
	const Type * type = param->getType ();
	return fromData (ptr, type);
}


Value fromData (const adv_ptr <uint8_t> & ptr, const Type * type) {
	if (typeid (*type) == typeid (PointerType)) {
		return Value (new PointerValue (ptr.reinterpret <Value> (), reinterpret_cast <const PointerType *> (type)));
	} else if (typeid (*type) == typeid (Struct)) {
		return Value (new TupleValue (ptr, reinterpret_cast <const Struct *> (type)));
	} else if (typeid (*type) == typeid (Array)) {
		return Value (new TupleValue (ptr, &reinterpret_cast <const Array *> (type)->getUnderlyingType ()));
	} else if (typeid (*type) == typeid (Number)) {
		const Number * num = reinterpret_cast <const Number *> (type);
		auto arith = num->getArithmaticType ();
		switch (num->calculateSize (sizeof (Value))) {
			case 1:
				if (arith == Type::INT) return Value (new GenericValue <int8_t> (ptr.reinterpret <int8_t> (), num));
				if (arith == Type::UINT) return Value (new GenericValue <uint8_t> (ptr.reinterpret <uint8_t> (), num));
			case 2:
				if (arith == Type::INT) return Value (new GenericValue <int16_t> (ptr.reinterpret <int16_t> (), num));
				if (arith == Type::UINT) return Value (new GenericValue <uint16_t> (ptr.reinterpret <uint16_t> (), num));
			case 4:
				if (arith == Type::INT) return Value (new GenericValue <int32_t> (ptr.reinterpret <int32_t> (), num));
				if (arith == Type::FPINT) return Value (new GenericValue <float> (ptr.reinterpret <float> (), num));
				if (arith == Type::UINT) return Value (new GenericValue <uint32_t> (ptr.reinterpret <uint32_t> (), num));
			case 8:
				if (arith == Type::INT) return Value (new GenericValue <int64_t> (ptr.reinterpret <int64_t> (), num));
				if (arith == Type::FPINT) return Value (new GenericValue <double> (ptr.reinterpret <double> (), num));
				if (arith == Type::UINT) return Value (new GenericValue <uint64_t> (ptr.reinterpret <uint64_t> (), num));
			default:
				abort ();
		}
	}
	abort ();
}


Value::Value () {}
Value::Value (AbstractValue * absVal) : val (absVal) {}
Value::Value (const Function * func) {abort ();}
Value::Value (const DynArray <Value> & members, const Struct * layout) {
	auto & mem = layout->getMemberArr ();
	assert (mem.size () == members.size ());
	adv_ptr <uint8_t> bytes (layout->calculateSize (sizeof (Value)), 0);
	auto itt_dest = bytes;
	for (size_t i=0, l=mem.size (); i<l; ++i) {
		const Parameter * param = mem [i];
		const Type * type = param->getType ();
		if (members [i]->getType () == type) {
			*fromData (itt_dest, type) = *members [i];
		} else if (members [i]->getType ()->getDereferenceType () == type) {
			*fromData (itt_dest, type) = *members [i]->dereference ();
		} else {
			abort ();
		}
		itt_dest = itt_dest + type->calculateSize (sizeof (Value));
	}
	val = std::shared_ptr <AbstractValue> (new TupleValue (bytes, layout));
}

Value::Value (double d) : val (new GenericValue <double> (d, Number::getNumberType (Type::FPINT, 64))) {}
Value::Value (int8_t d) : val (new GenericValue <int8_t> (d, Number::getNumberType (Type::INT, 8))) {}
Value::Value (int16_t d) : val (new GenericValue <int16_t> (d, Number::getNumberType (Type::INT, 16))) {}
Value::Value (int32_t d) : val (new GenericValue <int32_t> (d, Number::getNumberType (Type::INT, 32))) {}
Value::Value (int64_t d) : val (new GenericValue <int64_t> (d, Number::getNumberType (Type::INT, 64))) {}
Value::Value (uint8_t d) : val (new GenericValue <uint8_t> (d, Number::getNumberType (Type::UINT, 8))) {}
Value::Value (uint16_t d) : val (new GenericValue <uint16_t> (d, Number::getNumberType (Type::UINT, 16))) {}
Value::Value (uint32_t d) : val (new GenericValue <uint32_t> (d, Number::getNumberType (Type::UINT, 32))) {}
Value::Value (uint64_t d) : val (new GenericValue <uint64_t> (d, Number::getNumberType (Type::UINT, 64))) {}


Value Value::referenceTo () {
	adv_ptr <Value> ptr (new Value (*this));
	const Type * type = val->getType ()->getPointerTo ();
	return Value (new PointerValue (ptr, reinterpret_cast <const PointerType *> (type)));
}


Value Value::allocate (const Type * type, int64_t amount) {
	if (typeid (*type) == typeid (PointerType)) {
		adv_ptr <Value> ptr (amount);
		return Value (new PointerValue (ptr, reinterpret_cast <const PointerType *> (type)));
	} else if (typeid (*type) == typeid (Struct)) {
		return Value (new TupleValue (amount, reinterpret_cast <const Struct *> (type)));
	} else if (typeid (*type) == typeid (Array)) {
		return Value (new TupleValue (amount, &reinterpret_cast <const Array *> (type)->getUnderlyingType ()));
	} else if (typeid (*type) == typeid (Number)) {
		const Number * num = reinterpret_cast <const Number *> (type);
		auto arith = num->getArithmaticType ();
		switch (num->calculateSize (sizeof (Value))) {
			case 1:
				if (arith == Type::INT) return Value (new GenericValue <int8_t> (amount, 0, num));
				if (arith == Type::UINT) return Value (new GenericValue <uint8_t> (amount, 0, num));
			case 2:
				if (arith == Type::INT) return Value (new GenericValue <int16_t> (amount, 0, num));
				if (arith == Type::UINT) return Value (new GenericValue <uint16_t> (amount, 0, num));
			case 4:
				if (arith == Type::INT) return Value (new GenericValue <int32_t> (amount, 0, num));
				if (arith == Type::FPINT) return Value (new GenericValue <float> (amount, 0, num));
				if (arith == Type::UINT) return Value (new GenericValue <uint32_t> (amount, 0, num));
			case 8:
				if (arith == Type::INT) return Value (new GenericValue <int64_t> (amount, 0, num));
				if (arith == Type::FPINT) return Value (new GenericValue <double> (amount, 0, num));
				if (arith == Type::UINT) return Value (new GenericValue <uint64_t> (amount, 0, num));
			default:
				abort ();
		}
	}
	abort ();
}

}
