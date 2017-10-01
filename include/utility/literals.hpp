#pragma once


#include <gmpxx.h>
#include <memory>
#include <utility/array.hpp>


class AbstractLiteral {
	
	public:
	protected:
	private:
		
		
		
};

typedef std::shared_ptr <AbstractLiteral> Literal;

class NumericLiteral : public AbstractLiteral {
	
	public:
		
		virtual std::shared_ptr <NumericLiteral> operator + (const NumericLiteral & literal) const=0;
		virtual std::shared_ptr <NumericLiteral> operator - (const NumericLiteral & literal) const=0;
		virtual std::shared_ptr <NumericLiteral> operator * (const NumericLiteral & literal) const=0;
		virtual std::shared_ptr <NumericLiteral> operator / (const NumericLiteral & literal) const=0;
		virtual std::shared_ptr <NumericLiteral> operator | (const NumericLiteral & literal) const=0;
		virtual std::shared_ptr <NumericLiteral> operator & (const NumericLiteral & literal) const=0;
		virtual std::shared_ptr <NumericLiteral> operator ^ (const NumericLiteral & literal) const=0;
		
		virtual string toString (text::PrintMode mode) const=0;
		
		virtual bool operator not () const=0;
		virtual bool operator == (const NumericLiteral & literal) const=0;
		bool operator != (const NumericLiteral & literal) const {return not (*this == literal);}
		virtual bool operator < (const NumericLiteral & literal) const=0;
		virtual bool operator <= (const NumericLiteral & literal) const=0;
		bool operator > (const NumericLiteral & literal) const {return not (*this <= literal);}
		bool operator >= (const NumericLiteral & literal) const {return not (*this < literal);}
		
		virtual double toDouble () const=0;
		virtual float toFloat () const=0;
		virtual int64_t toInt64 () const=0;
		virtual mpq_class toMPQ () const=0;
		virtual uint64_t toUint64 () const=0;
		
	protected:
	private:
		
};

template <typename T, typename T_SFINAE = void>
class NumericLiteralTemplate : public NumericLiteral {
	
	public:
		
		virtual std::shared_ptr <NumericLiteral> operator + (const NumericLiteral & literal) const {
			if (typeid (literal) == typeid (*this)) {
				auto & rhs = static_cast <const NumericLiteralTemplate <T>&> (literal);
				return std::make_shared <NumericLiteralTemplate <T>> (number + rhs.number);
			}
			abort ();
		}
		virtual std::shared_ptr <NumericLiteral> operator - (const NumericLiteral & literal) const {
			if (typeid (literal) == typeid (*this)) {
				auto & rhs = static_cast <const NumericLiteralTemplate <T>&> (literal);
				return std::make_shared <NumericLiteralTemplate <T>> (number - rhs.number);
			}
			abort ();
		}
		virtual std::shared_ptr <NumericLiteral> operator * (const NumericLiteral & literal) const {
			if (typeid (literal) == typeid (*this)) {
				auto & rhs = static_cast <const NumericLiteralTemplate <T>&> (literal);
				return std::make_shared <NumericLiteralTemplate <T>> (number * rhs.number);
			}
			abort ();
		}
		virtual std::shared_ptr <NumericLiteral> operator / (const NumericLiteral & literal) const {
			if (typeid (literal) == typeid (*this)) {
				auto & rhs = static_cast <const NumericLiteralTemplate <T>&> (literal);
				return std::make_shared <NumericLiteralTemplate <T>> (number / rhs.number);
			}
			abort ();
		}
		virtual std::shared_ptr <NumericLiteral> operator | (const NumericLiteral & literal) const {
			if (typeid (literal) == typeid (*this)) {
				auto & rhs = static_cast <const NumericLiteralTemplate <T>&> (literal);
				return std::make_shared <NumericLiteralTemplate <T>> (number | rhs.number);
			}
			abort ();
		}
		virtual std::shared_ptr <NumericLiteral> operator & (const NumericLiteral & literal) const {
			if (typeid (literal) == typeid (*this)) {
				auto & rhs = static_cast <const NumericLiteralTemplate <T>&> (literal);
				return std::make_shared <NumericLiteralTemplate <T>> (number & rhs.number);
			}
			abort ();
		}
		virtual std::shared_ptr <NumericLiteral> operator ^ (const NumericLiteral & literal) const {
			if (typeid (literal) == typeid (*this)) {
				auto & rhs = static_cast <const NumericLiteralTemplate <T>&> (literal);
				return std::make_shared <NumericLiteralTemplate <T>> (number ^ rhs.number);
			}
			abort ();
		}
		
		virtual string toString (text::PrintMode mode) const {return std::to_string (number);}
		
		virtual bool operator not () const {return not number;}
		virtual bool operator == (const NumericLiteral & literal) const {
			if (typeid (literal) == typeid (*this)) {
				auto & rhs = static_cast <const NumericLiteralTemplate <T>&> (literal);
				return number == rhs.number;
			}
			return false;
		}
		virtual bool operator < (const NumericLiteral & literal) const {
			if (typeid (literal) == typeid (*this)) {
				auto & rhs = static_cast <const NumericLiteralTemplate <T>&> (literal);
				return number < rhs.number;
			}
			return false;
		}
		virtual bool operator <= (const NumericLiteral & literal) const {
			if (typeid (literal) == typeid (*this)) {
				auto & rhs = static_cast <const NumericLiteralTemplate <T>&> (literal);
				return number <= rhs.number;
			}
			return false;
		}
		
		virtual double toDouble () const {return double (number);}
		virtual float toFloat () const {return float (number);}
		virtual int64_t toInt64 () const {return int64_t (number);}
		virtual mpq_class toMPQ () const {return mpq_class (number);}
		virtual uint64_t toUint64 () const {return uint64_t (number);}
		
		NumericLiteralTemplate (const T & t) : number (t) {}
		
	protected:
	private:
		
		T number;
		
};

template <typename T>
class NumericLiteralTemplate <T, typename std::enable_if <std::is_floating_point <T>::value>::type> : public NumericLiteral {
	
	public:
		
		virtual std::shared_ptr <NumericLiteral> operator + (const NumericLiteral & literal) const {
			if (typeid (literal) == typeid (*this)) {
				auto & rhs = static_cast <const NumericLiteralTemplate <T>&> (literal);
				return std::make_shared <NumericLiteralTemplate <T>> (number + rhs.number);
			}
			abort ();
		}
		virtual std::shared_ptr <NumericLiteral> operator - (const NumericLiteral & literal) const {
			if (typeid (literal) == typeid (*this)) {
				auto & rhs = static_cast <const NumericLiteralTemplate <T>&> (literal);
				return std::make_shared <NumericLiteralTemplate <T>> (number - rhs.number);
			}
			abort ();
		}
		virtual std::shared_ptr <NumericLiteral> operator * (const NumericLiteral & literal) const {
			if (typeid (literal) == typeid (*this)) {
				auto & rhs = static_cast <const NumericLiteralTemplate <T>&> (literal);
				return std::make_shared <NumericLiteralTemplate <T>> (number * rhs.number);
			}
			abort ();
		}
		virtual std::shared_ptr <NumericLiteral> operator / (const NumericLiteral & literal) const {
			if (typeid (literal) == typeid (*this)) {
				auto & rhs = static_cast <const NumericLiteralTemplate <T>&> (literal);
				return std::make_shared <NumericLiteralTemplate <T>> (number / rhs.number);
			}
			abort ();
		}
		virtual std::shared_ptr <NumericLiteral> operator | (const NumericLiteral & literal) const {
			abort ();
		}
		virtual std::shared_ptr <NumericLiteral> operator & (const NumericLiteral & literal) const {
			abort ();
		}
		virtual std::shared_ptr <NumericLiteral> operator ^ (const NumericLiteral & literal) const {
			abort ();
		}
		
		virtual string toString (text::PrintMode mode) const {return std::to_string (number);}
		
		virtual bool operator not () const {return not number;}
		virtual bool operator == (const NumericLiteral & literal) const {
			if (typeid (literal) == typeid (*this)) {
				auto & rhs = static_cast <const NumericLiteralTemplate <T>&> (literal);
				return number == rhs.number;
			}
			return false;
		}
		virtual bool operator < (const NumericLiteral & literal) const {
			if (typeid (literal) == typeid (*this)) {
				auto & rhs = static_cast <const NumericLiteralTemplate <T>&> (literal);
				return number < rhs.number;
			}
			return false;
		}
		virtual bool operator <= (const NumericLiteral & literal) const {
			if (typeid (literal) == typeid (*this)) {
				auto & rhs = static_cast <const NumericLiteralTemplate <T>&> (literal);
				return number <= rhs.number;
			}
			return false;
		}
		
		virtual double toDouble () const {return double (number);}
		virtual float toFloat () const {return float (number);}
		virtual int64_t toInt64 () const {return int64_t (number);}
		virtual mpq_class toMPQ () const {return mpq_class (number);}
		virtual uint64_t toUint64 () const {return uint64_t (number);}
		
		NumericLiteralTemplate (const T & t) : number (t) {}
		
	protected:
	private:
		
		T number;
		
};

template <>
class NumericLiteralTemplate <mpq_class> : public NumericLiteral {
	
	public:
		
		virtual std::shared_ptr <NumericLiteral> operator + (const NumericLiteral & literal) const {
			if (typeid (literal) == typeid (*this)) {
				auto & rhs = static_cast <const NumericLiteralTemplate <mpq_class>&> (literal);
				return std::make_shared <NumericLiteralTemplate <mpq_class>> (number + rhs.number);
			}
			abort ();
		}
		virtual std::shared_ptr <NumericLiteral> operator - (const NumericLiteral & literal) const {
			if (typeid (literal) == typeid (*this)) {
				auto & rhs = static_cast <const NumericLiteralTemplate <mpq_class>&> (literal);
				return std::make_shared <NumericLiteralTemplate <mpq_class>> (number - rhs.number);
			}
			abort ();
		}
		virtual std::shared_ptr <NumericLiteral> operator * (const NumericLiteral & literal) const {
			if (typeid (literal) == typeid (*this)) {
				auto & rhs = static_cast <const NumericLiteralTemplate <mpq_class>&> (literal);
				return std::make_shared <NumericLiteralTemplate <mpq_class>> (number * rhs.number);
			}
			abort ();
		}
		virtual std::shared_ptr <NumericLiteral> operator / (const NumericLiteral & literal) const {
			if (typeid (literal) == typeid (*this)) {
				auto & rhs = static_cast <const NumericLiteralTemplate <mpq_class>&> (literal);
				return std::make_shared <NumericLiteralTemplate <mpq_class>> (number / rhs.number);
			}
			abort ();
		}
		virtual std::shared_ptr <NumericLiteral> operator | (const NumericLiteral & literal) const {abort ();}
		virtual std::shared_ptr <NumericLiteral> operator & (const NumericLiteral & literal) const {abort ();}
		virtual std::shared_ptr <NumericLiteral> operator ^ (const NumericLiteral & literal) const {abort ();}
		
		virtual string toString (text::PrintMode mode) const {return number.get_str ();}
		
		virtual bool operator not () const {return not number;}
		virtual bool operator == (const NumericLiteral & literal) const {
			if (typeid (literal) == typeid (*this)) {
				auto & rhs = static_cast <const NumericLiteralTemplate <mpq_class>&> (literal);
				return number == rhs.number;
			}
			return false;
		}
		virtual bool operator < (const NumericLiteral & literal) const {
			if (typeid (literal) == typeid (*this)) {
				auto & rhs = static_cast <const NumericLiteralTemplate <mpq_class>&> (literal);
				return number < rhs.number;
			}
			return false;
		}
		virtual bool operator <= (const NumericLiteral & literal) const {
			if (typeid (literal) == typeid (*this)) {
				auto & rhs = static_cast <const NumericLiteralTemplate <mpq_class>&> (literal);
				return number <= rhs.number;
			}
			return false;
		}
		
		virtual double toDouble () const {return number.get_d ();}
		virtual float toFloat () const {return number.get_d ();}
		virtual int64_t toInt64 () const {return number.get_num ().get_si () / number.get_den ().get_si ();}
		virtual mpq_class toMPQ () const {return number;}
		virtual uint64_t toUint64 () const {return number.get_num ().get_si () / number.get_den ().get_si ();}
		
		NumericLiteralTemplate (const mpq_class & t) : number (t) {}
		
	protected:
	private:
		
		mpq_class number;
		
};

class TupleLiteral : public AbstractLiteral {
	
	public:
	protected:
	private:
		
		BasicArray <std::shared_ptr <AbstractLiteral>> data;
		
};

class ReferenceLiteral : public AbstractLiteral {
	
	public:
	protected:
	private:
		
		AbstractLiteral * referencedData;
		
};

class ArrayLiteral : public AbstractLiteral {
	
	public:
	protected:
	private:
		
		std::shared_ptr <AbstractLiteral []> data;
		
};
